/*

Copyright (c) 2021 Addison Schuhardt

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include "ringbuf.h"

#include <stdlib.h>
#include <string.h>

typedef struct rb_block_t {
  size_t length;
  void* data;
} rb_block_t;

typedef struct rb_buffer_t {
  size_t capacity;
  size_t count;
  size_t block_size;
  rb_block_t** indices;
  rb_block_t* head;
  rb_block_t* tail;
  rb_block_t* blocks;
  rb_insert_callback_t on_insert;
  void* on_insert_state;
} rb_buffer_t;

static void set_head_and_tail(rb_buffer_t* buf) {
  buf->head = buf->indices[buf->capacity - 1];
  buf->tail = buf->indices[0];
}

static void rotate_buffer(rb_buffer_t* buf) {
  // rotate the block of pointers-to-blocks one unit toward the "head" of the
  // structure; the head (the oldest record) will be reused to point to the
  // newest insertion (the "tail")

  memmove(&buf->indices[1], buf->indices,
          sizeof(rb_block_t*) * (buf->capacity - 1));
  buf->indices[0] = buf->head;

  // update head and tail pointers post-rotation
  set_head_and_tail(buf);
}

static void write_to_tail(rb_buffer_t* buf, void* data, size_t len) {
  // copy the provided block of memory into the record to which the tail points

  rb_block_t* block = buf->tail;
  block->length = len;
  memcpy(block->data, data, len);
}

static void call_on_insert(rb_buffer_t* buf) {
  rb_block_t* block = buf->tail;
  buf->on_insert(buf->on_insert_state, block->data, block->length);
}

rb_buffer_t* rb_create(size_t capacity, size_t block_size) {
  rb_buffer_t* buf = malloc(sizeof(rb_buffer_t));

  if (buf == NULL) {
    return NULL;
  }

  buf->capacity = capacity;
  buf->count = 0;
  buf->block_size = block_size;
  buf->on_insert = NULL;
  buf->indices = malloc(capacity * sizeof(rb_block_t*));
  buf->blocks = calloc(capacity, sizeof(rb_block_t));

  // allocate each "record" block's buffer and set its length to 0
  for (size_t i = 0; i < capacity; ++i) {
    rb_block_t* block = &buf->blocks[i];

    buf->indices[i] = block;
    block->length = 0;
    block->data = calloc(block_size, sizeof(unsigned char));

    if (block->data == NULL) {
      rb_destroy(buf);
      return NULL;
    }
  }

  set_head_and_tail(buf);

  return buf;
}

void rb_destroy(rb_buffer_t* buf) {
  if (buf == NULL) {
    return;
  }

  if (buf->blocks != NULL) {
    for (size_t i = 0; i < buf->capacity; ++i) {
      if (buf->blocks[i].data != NULL) {
        free(buf->blocks[i].data);
      }
    }

    free(buf->blocks);
  }

  if (buf->indices != NULL) {
    free(buf->indices);
  }

  free(buf);
}

size_t rb_count(rb_buffer_t* buf) { return buf->count; }

void rb_insert(rb_buffer_t* buf, void* data, size_t length) {
  rotate_buffer(buf);

  write_to_tail(buf, data, length);

  if (buf->count < buf->capacity) {
    ++buf->count;
  }

  if (buf->on_insert != NULL) {
    call_on_insert(buf);
  }
}

void rb_get_tail(rb_buffer_t* buf, void** result, size_t* length) {
  rb_get_at(buf, 0, result, length);
}

void rb_get_at(rb_buffer_t* buf, size_t offset, void** result, size_t* length) {
  rb_block_t* block = buf->indices[offset % buf->capacity];
  *result = block->data;
  *length = block->length;
}

void rb_set_callback(rb_buffer_t* buf, void* state, rb_insert_callback_t cb) {
  buf->on_insert = cb;
  buf->on_insert_state = state;
}
