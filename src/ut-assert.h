#include <stdint.h>

#pragma once

#include "ut-object.h"

#define ut_assert_equal(a, b) _ut_assert_equal(__FILE__, __LINE__, #a, a, #b, b)
void _ut_assert_equal(const char *file, int line, const char *a_name,
                      UtObject *a_value, const char *b_name, UtObject *b_value);

#define ut_assert_is_error(value)                                              \
  _ut_assert_is_error(__FILE__, __LINE__, #value, value)
void _ut_assert_is_error(const char *file, int line, const char *name,
                         UtObject *value);

#define ut_assert_is_not_error(value)                                          \
  _ut_assert_is_not_error(__FILE__, __LINE__, #value, value)
void _ut_assert_is_not_error(const char *file, int line, const char *name,
                             UtObject *value);

#define ut_assert_int_equal(a, b)                                              \
  _ut_assert_int_equal(__FILE__, __LINE__, #a, a, #b, b)
void _ut_assert_int_equal(const char *file, int line, const char *a_name,
                          int a_value, const char *b_name, int b_value);

#define ut_assert_cstring_equal(a, b)                                          \
  _ut_assert_cstring_equal(__FILE__, __LINE__, #a, a, #b, b)
void _ut_assert_cstring_equal(const char *file, int line, const char *a_name,
                              const char *a_value, const char *b_name,
                              const char *b_value);

#define ut_assert_uint8_list_equal(a, b, b_length)                             \
  _ut_assert_uint8_list_equal(__FILE__, __LINE__, #a, a, b, b_length)
void _ut_assert_uint8_list_equal(const char *file, int line, const char *a_name,
                                 UtObject *a_value, uint8_t *b_value,
                                 size_t b_length);

#define ut_assert_uint8_array_equal(a, a_length, b, b_length)                  \
  _ut_assert_uint8_array_equal(__FILE__, __LINE__, #a, a, a_length, b, b_length)
void _ut_assert_uint8_array_equal(const char *file, int line,
                                  const char *a_name, const uint8_t *a_value,
                                  size_t a_length, const uint8_t *b_value,
                                  size_t b_length);

#define ut_assert_uint16_array_equal(a, a_length, b, b_length)                 \
  _ut_assert_uint16_array_equal(__FILE__, __LINE__, #a, a, a_length, b,        \
                                b_length)
void _ut_assert_uint16_array_equal(const char *file, int line,
                                   const char *a_name, const uint16_t *a_value,
                                   size_t a_length, const uint16_t *b_value,
                                   size_t b_length);
