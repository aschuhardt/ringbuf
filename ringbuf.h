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

#ifndef RINGBUF_H
#define RINGBUF_H

#include <stddef.h>

typedef struct rb_buffer_t rb_buffer_t;

typedef void (*rb_insert_callback_t)(void* user_state, void* data,
                                     size_t length);

/*
 * Initializes a new instance of the ring buffer structure with the provided
 * `capacity` (number of records total) and `block_size` (max size of each
 * record).
 *
 * Returns the newly-created object, or NULL if an out-of-memory error occurred.
 */
rb_buffer_t* rb_create(size_t capacity, size_t block_size);

/*
 * Frees resources allocated by the data structure
 */
void rb_destroy(rb_buffer_t* buf);

/*
 * Returns the number of records currently present in the data structure.  This
 * will always be less-than or equal-to the value of `capacity` passed to
 * `rb_create`.
 */
size_t rb_count(rb_buffer_t* buf);

/*
 * Inserts a new record into the data structure, consisting of a block of memory
 * `length` bytes in length and pointed to by `data`
 */
void rb_insert(rb_buffer_t* buf, void* data, size_t length);

/*
 * Assigns a pointer to the most recently-inserted record data to `result`, or
 * NULL if there are no records in the data structure.
 *
 * Assigns the length, in bytes, of the tail record to address pointed to by
 * `length`, or 0 if there are no records in the data structure.
 */
void rb_get_tail(rb_buffer_t* buf, void** result, size_t* length);

/*
 * Assigns a pointer to the record data at `offset` from the tail to `result`,
 * or NULL if there is no record at that offset.
 *
 * Assigns the length, in bytes, of the record at `offset` to the address
 * pointed to by `length`, or 0 if there is no record at that offset.
 *
 * The record at offset 0 is the most recently-inserted record (the
 * tail), at offset 1 is the second-most recently-inserted, etc.
 */
void rb_get_at(rb_buffer_t* buf, size_t offset, void** result, size_t* length);

/*
 * Sets the callback function to be invoked upon insertion of a new record into
 * the data structure.
 *
 * The call will be passed a pointer to a user-defined state object, a pointer
 * to the record's data, and the length, in bytes, of the new data.
 */
void rb_set_callback(rb_buffer_t* buf, void* state, rb_insert_callback_t cb);

#endif
