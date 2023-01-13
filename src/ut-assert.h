#include <stdint.h>

#pragma once

#include "ut-object.h"

/// Asserts that the two objects [a] and [b] are equal.
#define ut_assert_equal(a, b) _ut_assert_equal(__FILE__, __LINE__, #a, a, #b, b)
void _ut_assert_equal(const char *file, int line, const char *a_name,
                      UtObject *a_value, const char *b_name, UtObject *b_value);

/// Asserts that [value] is a [UtError].
#define ut_assert_is_error(value)                                              \
  _ut_assert_is_error(__FILE__, __LINE__, #value, value)
void _ut_assert_is_error(const char *file, int line, const char *name,
                         UtObject *value);

/// Asserts that [value] is not a [UtError].
#define ut_assert_is_not_error(value)                                          \
  _ut_assert_is_not_error(__FILE__, __LINE__, #value, value)
void _ut_assert_is_not_error(const char *file, int line, const char *name,
                             UtObject *value);

/// Asserts that the two integers [a] and [b] are equal.
#define ut_assert_int_equal(a, b)                                              \
  _ut_assert_int_equal(__FILE__, __LINE__, #a, a, #b, b)
void _ut_assert_int_equal(const char *file, int line, const char *a_name,
                          int a_value, const char *b_name, int b_value);

/// Asserts that the two floating point numbers [a] and [b] are equal.
#define ut_assert_float_equal(a, b)                                            \
  _ut_assert_float_equal(__FILE__, __LINE__, #a, a, #b, b)
void _ut_assert_float_equal(const char *file, int line, const char *a_name,
                            double a_value, const char *b_name, double b_value);

/// Asserts that the two C strings [a] and [b] are equal.
#define ut_assert_cstring_equal(a, b)                                          \
  _ut_assert_cstring_equal(__FILE__, __LINE__, #a, a, #b, b)
void _ut_assert_cstring_equal(const char *file, int line, const char *a_name,
                              const char *a_value, const char *b_name,
                              const char *b_value);

/// Asserts that the [UtUint8List] object [a] has the same contents a the C
/// array [b].
#define ut_assert_uint8_list_equal(a, b, b_length)                             \
  _ut_assert_uint8_list_equal(__FILE__, __LINE__, #a, a, b, b_length)
void _ut_assert_uint8_list_equal(const char *file, int line, const char *a_name,
                                 UtObject *a_value, uint8_t *b_value,
                                 size_t b_length);

/// Asserts that the [UtUint8List] object [a] has the same contents a the
/// hexadecimal string [b_hex].
#define ut_assert_uint8_list_equal_hex(a, b_hex)                               \
  _ut_assert_uint8_list_equal_hex(__FILE__, __LINE__, #a, a, b_hex)
void _ut_assert_uint8_list_equal_hex(const char *file, int line,
                                     const char *a_name, UtObject *a_value,
                                     const char *b_hex);

/// Asserts that the two C arrays [a] and [b] have the same contents.
#define ut_assert_uint8_array_equal(a, a_length, b, b_length)                  \
  _ut_assert_uint8_array_equal(__FILE__, __LINE__, #a, a, a_length, b, b_length)
void _ut_assert_uint8_array_equal(const char *file, int line,
                                  const char *a_name, const uint8_t *a_value,
                                  size_t a_length, const uint8_t *b_value,
                                  size_t b_length);

/// Asserts that the two C arrays [a] and [b] have the same contents.
#define ut_assert_uint16_array_equal(a, a_length, b, b_length)                 \
  _ut_assert_uint16_array_equal(__FILE__, __LINE__, #a, a, a_length, b,        \
                                b_length)
void _ut_assert_uint16_array_equal(const char *file, int line,
                                   const char *a_name, const uint16_t *a_value,
                                   size_t a_length, const uint16_t *b_value,
                                   size_t b_length);
