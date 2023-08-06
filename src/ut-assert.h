#include <stdint.h>

#pragma once

#include "ut-object.h"

/// Asserts that [value] is true.
#define ut_assert_true(value) _ut_assert_true(__FILE__, __LINE__, #value, value)
void _ut_assert_true(const char *file, int line, const char *value_name,
                     bool value);

/// Asserts that [value] is false.
#define ut_assert_false(value)                                                 \
  _ut_assert_false(__FILE__, __LINE__, #value, value)
void _ut_assert_false(const char *file, int line, const char *value_name,
                      bool value);

/// Asserts that [value] is NULL.
#define ut_assert_null(value) _ut_assert_null(__FILE__, __LINE__, #value, value)
void _ut_assert_null(const char *file, int line, const char *value_name,
                     const void *value);

/// Asserts that [value] is not NULL.
#define ut_assert_non_null(value)                                              \
  _ut_assert_non_null(__FILE__, __LINE__, #value, value)
void _ut_assert_non_null(const char *file, int line, const char *value_name,
                         const void *value);

/// Asserts that the two objects [a] and [b] are equal.
#define ut_assert_equal(a, b) _ut_assert_equal(__FILE__, __LINE__, #a, a, #b, b)
void _ut_assert_equal(const char *file, int line, const char *a_name,
                      UtObject *a_value, const char *b_name, UtObject *b_value);

/// Asserts that [value] is a [UtError].
#define ut_assert_is_error(value)                                              \
  _ut_assert_is_error(__FILE__, __LINE__, #value, value)
void _ut_assert_is_error(const char *file, int line, const char *name,
                         UtObject *value);

/// Asserts that [value] is a [UtError] with [description].
#define ut_assert_is_error_with_description(value, description)                \
  _ut_assert_is_error_with_description(__FILE__, __LINE__, #value, value,      \
                                       description)
void _ut_assert_is_error_with_description(const char *file, int line,
                                          const char *name, UtObject *value,
                                          const char *description);

/// Asserts that [value] is not a [UtError].
#define ut_assert_is_not_error(value)                                          \
  _ut_assert_is_not_error(__FILE__, __LINE__, #value, value)
void _ut_assert_is_not_error(const char *file, int line, const char *name,
                             UtObject *value);

/// Asserts that the two integers [a] and [b] are equal.
#define ut_assert_int_equal(a, b)                                              \
  _ut_assert_int_equal(__FILE__, __LINE__, #a, a, #b, b)
void _ut_assert_int_equal(const char *file, int line, const char *a_name,
                          int64_t a_value, const char *b_name, int64_t b_value);

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

/// Asserts that the [UtBitList] object [a] has the same contents a the
/// binary string [b_bin].
#define ut_assert_bit_list_equal_bin(a, b_bin)                                 \
  _ut_assert_bit_list_equal_bin(__FILE__, __LINE__, #a, a, b_bin)
void _ut_assert_bit_list_equal_bin(const char *file, int line,
                                   const char *a_name, UtObject *a_value,
                                   const char *b_bin);

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

/// Asserts that the [UtUint16List] object [a] has the same contents a the C
/// array [b].
#define ut_assert_uint16_list_equal(a, b, b_length)                            \
  _ut_assert_uint16_list_equal(__FILE__, __LINE__, #a, a, b, b_length)
void _ut_assert_uint16_list_equal(const char *file, int line,
                                  const char *a_name, UtObject *a_value,
                                  uint16_t *b_value, size_t b_length);

/// Asserts that the [UtUint16List] object [a] has the same contents a the
/// hexadecimal string [b_hex].
#define ut_assert_uint16_list_equal_hex(a, b_hex)                              \
  _ut_assert_uint16_list_equal_hex(__FILE__, __LINE__, #a, a, b_hex)
void _ut_assert_uint16_list_equal_hex(const char *file, int line,
                                      const char *a_name, UtObject *a_value,
                                      const char *b_hex);

/// Asserts that the [UtInt16List] object [a] has the same contents a the C
/// array [b].
#define ut_assert_int16_list_equal(a, b, b_length)                             \
  _ut_assert_int16_list_equal(__FILE__, __LINE__, #a, a, b, b_length)
void _ut_assert_int16_list_equal(const char *file, int line, const char *a_name,
                                 UtObject *a_value, int16_t *b_value,
                                 size_t b_length);

/// Asserts that the [UtUint32List] object [a] has the same contents a the C
/// array [b].
#define ut_assert_uint32_list_equal(a, b, b_length)                            \
  _ut_assert_uint32_list_equal(__FILE__, __LINE__, #a, a, b, b_length)
void _ut_assert_uint32_list_equal(const char *file, int line,
                                  const char *a_name, UtObject *a_value,
                                  uint32_t *b_value, size_t b_length);

/// Asserts that the [UtInt32List] object [a] has the same contents a the C
/// array [b].
#define ut_assert_int32_list_equal(a, b, b_length)                             \
  _ut_assert_int32_list_equal(__FILE__, __LINE__, #a, a, b, b_length)
void _ut_assert_int32_list_equal(const char *file, int line, const char *a_name,
                                 UtObject *a_value, int32_t *b_value,
                                 size_t b_length);

/// Asserts that the [UtUint64List] object [a] has the same contents a the C
/// array [b].
#define ut_assert_uint64_list_equal(a, b, b_length)                            \
  _ut_assert_uint64_list_equal(__FILE__, __LINE__, #a, a, b, b_length)
void _ut_assert_uint64_list_equal(const char *file, int line,
                                  const char *a_name, UtObject *a_value,
                                  uint64_t *b_value, size_t b_length);

/// Asserts that the [UtInt64List] object [a] has the same contents a the C
/// array [b].
#define ut_assert_int64_list_equal(a, b, b_length)                             \
  _ut_assert_int64_list_equal(__FILE__, __LINE__, #a, a, b, b_length)
void _ut_assert_int64_list_equal(const char *file, int line, const char *a_name,
                                 UtObject *a_value, int64_t *b_value,
                                 size_t b_length);

/// Asserts that the [UtFloat32List] object [a] has the same contents a the C
/// array [b].
#define ut_assert_float32_list_equal(a, b, b_length)                           \
  _ut_assert_float32_list_equal(__FILE__, __LINE__, #a, a, b, b_length)
void _ut_assert_float32_list_equal(const char *file, int line,
                                   const char *a_name, UtObject *a_value,
                                   float *b_value, size_t b_length);

/// Asserts that the [UtFloat64List] object [a] has the same contents a the C
/// array [b].
#define ut_assert_float64_list_equal(a, b, b_length)                           \
  _ut_assert_float64_list_equal(__FILE__, __LINE__, #a, a, b, b_length)
void _ut_assert_float64_list_equal(const char *file, int line,
                                   const char *a_name, UtObject *a_value,
                                   double *b_value, size_t b_length);
