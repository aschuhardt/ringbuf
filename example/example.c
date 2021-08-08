#include <stdio.h>
#include <string.h>

#include "ringbuf.h"

#define MAX_LINE_SIZE 128
#define CAPACITY 16

int line_number = 0;

void report_line_size(void* state, void* data, size_t length) {
  printf("Line %d: %zu bytes\n", line_number++, length);
}

int main() {
  // initialize the ring buffer
  rb_buffer_t* buf = rb_create(CAPACITY, MAX_LINE_SIZE);

  // set a callback function that the ring buffer will call after each insertion
  rb_set_callback(buf, NULL, report_line_size);

  FILE* fd = fopen("proserpine.txt", "r");

  // insert each line of the file into the ring buffer
  char line_in[MAX_LINE_SIZE];
  while (fgets(&line_in[0], MAX_LINE_SIZE, fd) != NULL) {
    rb_insert(buf, &line_in[0], strnlen(&line_in[0], MAX_LINE_SIZE));
  }

  fclose(fd);

  // write out h
  char line_out[MAX_LINE_SIZE];
  for (int offset = CAPACITY - 1; offset >= 0; --offset) {
    void* stored;
    size_t length;

    rb_get_at(buf, offset, &stored, &length);

    if (stored != NULL && length > 0) {
      memset(&line_out[0], 0, sizeof(line_out) / sizeof(char));
      snprintf(&line_out[0], length, "%s", (char*)stored);
      printf("%d: %s\n", offset, &line_out[0]);
    }
  }

  rb_destroy(buf);

  return 0;
}
