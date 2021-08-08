#include <string.h>

#include "ctest/ctest.h"
#include "ringbuf.h"

#define CAPACITY 128
#define BLOCK_SIZE 256

CTEST_DATA(ringbuf) { rb_buffer_t* buf; };

CTEST_SETUP(ringbuf) { data->buf = rb_create(CAPACITY, BLOCK_SIZE); }

CTEST_TEARDOWN(ringbuf) { rb_destroy(data->buf); }

CTEST2(ringbuf, create_returns_not_null) { ASSERT_NOT_NULL(data->buf); }

CTEST2(ringbuf, default_count_is_zero) { ASSERT_EQUAL(0, rb_count(data->buf)); }

CTEST2(ringbuf, tail_reflects_last_inserted) {
  const char str[] =
      "How does the wall keep us free, my children, my children?";
  rb_insert(data->buf, (void*)&str[0], sizeof(str) / sizeof(char));

  void* result;
  size_t len;
  rb_get_tail(data->buf, &result, &len);

  ASSERT_NOT_NULL(result);
  ASSERT_STR(&str[0], (char*)result);
}

CTEST2(ringbuf, insertion_order_is_preserved) {
  const char* values[] = {"first", "second", "third"};

  rb_insert(data->buf, (void*)values[0], strlen(values[0]));
  rb_insert(data->buf, (void*)values[1], strlen(values[1]));
  rb_insert(data->buf, (void*)values[2], strlen(values[2]));

  void* result;
  size_t len;

  rb_get_at(data->buf, 0, &result, &len);
  ASSERT_NOT_NULL(result);
  ASSERT_STR(values[2], (char*)result);

  rb_get_at(data->buf, 1, &result, &len);
  ASSERT_NOT_NULL(result);
  ASSERT_STR(values[1], (char*)result);

  rb_get_at(data->buf, 2, &result, &len);
  ASSERT_NOT_NULL(result);
  ASSERT_STR(values[0], (char*)result);
}

CTEST2(ringbuf, invalid_offset_has_zero_length) {
  void* result;
  size_t len;
  rb_get_at(data->buf, 0, &result, &len);
  ASSERT_EQUAL(0, len);
}
