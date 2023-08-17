#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ut.h"

static char *append_string(char *value, const char *suffix) {
  size_t value_length = value != NULL ? ut_cstring_get_length(value) : 0;
  size_t suffix_length = ut_cstring_get_length(suffix);

  value = realloc(value, value_length + suffix_length + 1);
  memcpy(value + value_length, suffix, suffix_length + 1);

  return value;
}

static char *escape_character(char *value, char c) {
  size_t value_length = ut_cstring_get_length(value);

  value_length++;
  value = realloc(value, value_length);
  if (c < 32 || c >= 127) {
    char text[5];
    snprintf(text, 5, "\\x%02x", c);
    value = append_string(value, text);
  }
  if (c == '\t') {
    value = append_string(value, "\\t");
  } else {
    char text[2] = {c, '\0'};
    value = append_string(value, text);
  }

  return value;
}

static char *escape_string(const char *value) {
  if (value == NULL) {
    return ut_cstring_new("NULL");
  }
  char *escaped = append_string(NULL, "\"");
  for (const char *c = value; *c != '\0'; c++) {
    escaped = escape_character(escaped, *c);
  }
  return append_string(escaped, "\"");
}

void _ut_assert_true(const char *file, int line, const char *value_name,
                     bool value) {
  if (value) {
    return;
  }

  fprintf(stderr, "%s:%d Expected %s to be TRUE\n", file, line, value_name);

  abort();
}

void _ut_assert_false(const char *file, int line, const char *value_name,
                      bool value) {
  if (!value) {
    return;
  }

  fprintf(stderr, "%s:%d Expected %s to be FALSE\n", file, line, value_name);

  abort();
}

void _ut_assert_null_cstring(const char *file, int line, const char *value_name,
                             const char *value) {
  if (value == NULL) {
    return;
  }

  fprintf(stderr,
          "%s:%d Expected %s to be NULL:\n"
          "  %s\n",
          file, line, value_name, value);

  abort();
}

void _ut_assert_null_object(const char *file, int line, const char *value_name,
                            UtObject *value) {
  if (value == NULL) {
    return;
  }

  ut_cstring_ref value_string = ut_object_to_string(value);
  fprintf(stderr,
          "%s:%d Expected %s to be NULL:\n"
          "  %s\n",
          file, line, value_name, value_string);

  abort();
}

void _ut_assert_non_null_object(const char *file, int line,
                                const char *value_name, UtObject *value) {
  if (value != NULL) {
    return;
  }

  fprintf(stderr, "%s:%d Expected %s to not be NULL\n", file, line, value_name);

  abort();
}

void _ut_assert_equal(const char *file, int line, const char *a_name,
                      UtObject *a_value, const char *b_name,
                      UtObject *b_value) {
  if (a_value == NULL && b_value == NULL) {
    return;
  }
  if (a_value != NULL && b_value != NULL && ut_object_equal(a_value, b_value)) {
    return;
  }

  ut_cstring_ref a_value_string =
      a_value != NULL ? ut_object_to_string(a_value) : ut_cstring_new("NULL");
  ut_cstring_ref b_value_string =
      b_value != NULL ? ut_object_to_string(b_value) : ut_cstring_new("NULL");
  fprintf(stderr,
          "%s:%d Objects %s and %s are not equal:\n"
          "  %s\n"
          "  %s\n",
          file, line, a_name, b_name, a_value_string, b_value_string);

  abort();
}

void _ut_assert_is_error(const char *file, int line, const char *name,
                         UtObject *value) {
  if (value != NULL && ut_object_implements_error(value)) {
    return;
  }

  ut_cstring_ref value_string =
      value != NULL ? ut_object_to_string(value) : ut_cstring_new("(NULL)");
  fprintf(stderr,
          "%s:%d Object %s is not an error:\n"
          "  %s\n",
          file, line, name, value_string);

  abort();
}

void _ut_assert_is_error_with_description(const char *file, int line,
                                          const char *name, UtObject *value,
                                          const char *description) {
  _ut_assert_is_error(file, line, name, value);

  ut_cstring_ref value_description = ut_error_get_description(value);
  if (ut_cstring_equal(description, value_description)) {
    return;
  }

  ut_cstring_ref value_string =
      value != NULL ? ut_object_to_string(value) : ut_cstring_new("(NULL)");
  fprintf(stderr,
          "%s:%d Error %s doesn't have expected description:\n"
          "  %s\n"
          "  %s\n",
          file, line, name, value_description, description);

  abort();
}

void _ut_assert_is_not_error(const char *file, int line, const char *name,
                             UtObject *value) {
  if (!ut_object_implements_error(value)) {
    return;
  }

  ut_cstring_ref value_string = ut_object_to_string(value);
  fprintf(stderr,
          "%s:%d Object %s is an error:\n"
          "  %s\n",
          file, line, name, value_string);

  abort();
}

void _ut_assert_int_equal(const char *file, int line, const char *a_name,
                          int64_t a_value, const char *b_name,
                          int64_t b_value) {
  if (a_value == b_value) {
    return;
  }

  fprintf(stderr,
          "%s:%d Integers %s and %s are not equal:\n"
          "  %li\n"
          "  %li\n",
          file, line, a_name, b_name, a_value, b_value);

  abort();
}

void _ut_assert_float_equal(const char *file, int line, const char *a_name,
                            double a_value, const char *b_name,
                            double b_value) {
  if (a_value == b_value) {
    return;
  }

  fprintf(stderr,
          "%s:%d Floating point numbers %s and %s are not equal:\n"
          "  %f\n"
          "  %f\n",
          file, line, a_name, b_name, a_value, b_value);

  abort();
}

void _ut_assert_cstring_equal(const char *file, int line, const char *a_name,
                              const char *a_value, const char *b_name,
                              const char *b_value) {
  if (a_value == NULL && b_value == NULL) {
    return;
  }
  if (a_value != NULL && b_value != NULL &&
      ut_cstring_equal(a_value, b_value)) {
    return;
  }

  ut_cstring_ref escaped_a_value = escape_string(a_value);
  ut_cstring_ref escaped_b_value = escape_string(b_value);
  fprintf(stderr,
          "%s:%d Strings %s and %s are not equal:\n"
          "  %s\n"
          "  %s\n"
          "  ",
          file, line, a_name, b_name, escaped_a_value, escaped_b_value);
  for (size_t i = 0; escaped_a_value[i] != '\0' && escaped_b_value[i] != '\0' &&
                     escaped_a_value[i] == escaped_b_value[i];
       i++) {
    fprintf(stderr, " ");
  }
  fprintf(stderr, "^\n");

  abort();
}

void _ut_assert_bit_list_equal_bin(const char *file, int line,
                                   const char *a_name, UtObject *a_value,
                                   const char *b_bin) {
  UtObjectRef b_value = ut_bit_list_new_msb_from_bin_string(b_bin);
  size_t length = ut_bit_list_get_length(a_value);
  if (length == ut_bit_list_get_length(b_value)) {
    bool match = true;
    for (size_t i = 0; i < length; i++) {
      if (ut_bit_list_get_element(a_value, i) !=
          ut_bit_list_get_element(b_value, i)) {
        match = false;
        break;
      }
    }

    if (match) {
      return;
    }
  }

  ut_cstring_ref a_value_string = ut_object_to_string(a_value);
  ut_cstring_ref b_value_string = ut_object_to_string(b_value);
  fprintf(stderr,
          "%s:%d List %s doesn't have expected content:\n"
          "  %s\n"
          "  %s\n",
          file, line, a_name, a_value_string, b_value_string);

  abort();
}

void _ut_assert_uint8_list_equal(const char *file, int line, const char *a_name,
                                 UtObject *a_value, uint8_t *b_value,
                                 size_t b_length) {
  if (ut_list_get_length(a_value) == b_length) {
    bool match = true;
    for (size_t i = 0; i < b_length; i++) {
      if (ut_uint8_list_get_element(a_value, i) != b_value[i]) {
        match = false;
        break;
      }
    }

    if (match) {
      return;
    }
  }

  UtObjectRef b_value_string = ut_string_new("<uint8>[");
  for (size_t i = 0; i < b_length; i++) {
    if (i != 0) {
      ut_string_append(b_value_string, ", ");
    }
    ut_string_append_printf(b_value_string, "%d", b_value[i]);
  }
  ut_string_append(b_value_string, "]");

  ut_cstring_ref a_value_string = ut_object_to_string(a_value);
  fprintf(stderr,
          "%s:%d List %s doesn't have expected content:\n"
          "  %s\n"
          "  %s\n",
          file, line, a_name, a_value_string,
          ut_string_get_text(b_value_string));

  abort();
}

void _ut_assert_uint8_list_equal_hex(const char *file, int line,
                                     const char *a_name, UtObject *a_value,
                                     const char *b_hex) {
  UtObjectRef b_value = ut_uint8_list_new_from_hex_string(b_hex);
  size_t length = ut_list_get_length(a_value);
  if (length == ut_list_get_length(b_value)) {
    bool match = true;
    for (size_t i = 0; i < length; i++) {
      if (ut_uint8_list_get_element(a_value, i) !=
          ut_uint8_list_get_element(b_value, i)) {
        match = false;
        break;
      }
    }

    if (match) {
      return;
    }
  }

  ut_cstring_ref a_value_string = ut_uint8_list_to_hex_string(a_value);
  fprintf(stderr,
          "%s:%d List %s doesn't have expected content:\n"
          "  h\"%s\"\n"
          "  h\"%s\"\n",
          file, line, a_name, a_value_string, b_hex);

  abort();
}

void _ut_assert_uint8_array_equal(const char *file, int line,
                                  const char *a_name, const uint8_t *a_value,
                                  size_t a_length, const uint8_t *b_value,
                                  size_t b_length) {
  if (a_length == b_length) {
    bool match = true;
    for (size_t i = 0; i < a_length; i++) {
      if (a_value[i] != b_value[i]) {
        match = false;
        break;
      }
    }

    if (match) {
      return;
    }
  }

  UtObjectRef a_value_string = ut_string_new("<uint8>[");
  UtObjectRef b_value_string = ut_string_new("<uint8>[");
  for (size_t i = 0; i < a_length; i++) {
    if (i != 0) {
      ut_string_append(a_value_string, ", ");
    }
    ut_string_append_printf(a_value_string, "%d", a_value[i]);
  }
  for (size_t i = 0; i < b_length; i++) {
    if (i != 0) {
      ut_string_append(b_value_string, ", ");
    }
    ut_string_append_printf(b_value_string, "%d", b_value[i]);
  }
  ut_string_append(a_value_string, "]");
  ut_string_append(b_value_string, "]");

  fprintf(stderr,
          "%s:%d Array %s doesn't have expected content:\n"
          "  %s\n"
          "  %s\n",
          file, line, a_name, ut_string_get_text(a_value_string),
          ut_string_get_text(b_value_string));

  abort();
}

void _ut_assert_uint16_list_equal(const char *file, int line,
                                  const char *a_name, UtObject *a_value,
                                  uint16_t *b_value, size_t b_length) {
  if (ut_list_get_length(a_value) == b_length) {
    bool match = true;
    for (size_t i = 0; i < b_length; i++) {
      if (ut_uint16_list_get_element(a_value, i) != b_value[i]) {
        match = false;
        break;
      }
    }

    if (match) {
      return;
    }
  }

  UtObjectRef b_value_string = ut_string_new("<uint16>[");
  for (size_t i = 0; i < b_length; i++) {
    if (i != 0) {
      ut_string_append(b_value_string, ", ");
    }
    ut_string_append_printf(b_value_string, "%u", b_value[i]);
  }
  ut_string_append(b_value_string, "]");

  ut_cstring_ref a_value_string = ut_object_to_string(a_value);
  fprintf(stderr,
          "%s:%d List %s doesn't have expected content:\n"
          "  %s\n"
          "  %s\n",
          file, line, a_name, a_value_string,
          ut_string_get_text(b_value_string));

  abort();
}

void _ut_assert_uint16_list_equal_hex(const char *file, int line,
                                      const char *a_name, UtObject *a_value,
                                      const char *b_hex) {
  UtObjectRef b_value = ut_uint16_list_new_from_hex_string(b_hex);
  size_t length = ut_list_get_length(a_value);
  if (length == ut_list_get_length(b_value)) {
    bool match = true;
    for (size_t i = 0; i < length; i++) {
      if (ut_uint16_list_get_element(a_value, i) !=
          ut_uint16_list_get_element(b_value, i)) {
        match = false;
        break;
      }
    }

    if (match) {
      return;
    }
  }

  ut_cstring_ref a_value_string = ut_uint16_list_to_hex_string(a_value);
  fprintf(stderr,
          "%s:%d List %s doesn't have expected content:\n"
          "  h\"%s\"\n"
          "  h\"%s\"\n",
          file, line, a_name, a_value_string, b_hex);

  abort();
}

void _ut_assert_int16_list_equal(const char *file, int line, const char *a_name,
                                 UtObject *a_value, int16_t *b_value,
                                 size_t b_length) {
  if (ut_list_get_length(a_value) == b_length) {
    bool match = true;
    for (size_t i = 0; i < b_length; i++) {
      if (ut_int16_list_get_element(a_value, i) != b_value[i]) {
        match = false;
        break;
      }
    }

    if (match) {
      return;
    }
  }

  UtObjectRef b_value_string = ut_string_new("<int16>[");
  for (size_t i = 0; i < b_length; i++) {
    if (i != 0) {
      ut_string_append(b_value_string, ", ");
    }
    ut_string_append_printf(b_value_string, "%d", b_value[i]);
  }
  ut_string_append(b_value_string, "]");

  ut_cstring_ref a_value_string = ut_object_to_string(a_value);
  fprintf(stderr,
          "%s:%d List %s doesn't have expected content:\n"
          "  %s\n"
          "  %s\n",
          file, line, a_name, a_value_string,
          ut_string_get_text(b_value_string));

  abort();
}

void _ut_assert_uint32_list_equal(const char *file, int line,
                                  const char *a_name, UtObject *a_value,
                                  uint32_t *b_value, size_t b_length) {
  if (ut_list_get_length(a_value) == b_length) {
    bool match = true;
    for (size_t i = 0; i < b_length; i++) {
      if (ut_uint32_list_get_element(a_value, i) != b_value[i]) {
        match = false;
        break;
      }
    }

    if (match) {
      return;
    }
  }

  UtObjectRef b_value_string = ut_string_new("<uint32>[");
  for (size_t i = 0; i < b_length; i++) {
    if (i != 0) {
      ut_string_append(b_value_string, ", ");
    }
    ut_string_append_printf(b_value_string, "%u", b_value[i]);
  }
  ut_string_append(b_value_string, "]");

  ut_cstring_ref a_value_string = ut_object_to_string(a_value);
  fprintf(stderr,
          "%s:%d List %s doesn't have expected content:\n"
          "  %s\n"
          "  %s\n",
          file, line, a_name, a_value_string,
          ut_string_get_text(b_value_string));

  abort();
}

void _ut_assert_int32_list_equal(const char *file, int line, const char *a_name,
                                 UtObject *a_value, int32_t *b_value,
                                 size_t b_length) {
  if (ut_list_get_length(a_value) == b_length) {
    bool match = true;
    for (size_t i = 0; i < b_length; i++) {
      if (ut_int32_list_get_element(a_value, i) != b_value[i]) {
        match = false;
        break;
      }
    }

    if (match) {
      return;
    }
  }

  UtObjectRef b_value_string = ut_string_new("<int32>[");
  for (size_t i = 0; i < b_length; i++) {
    if (i != 0) {
      ut_string_append(b_value_string, ", ");
    }
    ut_string_append_printf(b_value_string, "%d", b_value[i]);
  }
  ut_string_append(b_value_string, "]");

  ut_cstring_ref a_value_string = ut_object_to_string(a_value);
  fprintf(stderr,
          "%s:%d List %s doesn't have expected content:\n"
          "  %s\n"
          "  %s\n",
          file, line, a_name, a_value_string,
          ut_string_get_text(b_value_string));

  abort();
}

void _ut_assert_uint64_list_equal(const char *file, int line,
                                  const char *a_name, UtObject *a_value,
                                  uint64_t *b_value, size_t b_length) {
  if (ut_list_get_length(a_value) == b_length) {
    bool match = true;
    for (size_t i = 0; i < b_length; i++) {
      if (ut_uint64_list_get_element(a_value, i) != b_value[i]) {
        match = false;
        break;
      }
    }

    if (match) {
      return;
    }
  }

  UtObjectRef b_value_string = ut_string_new("<uint64>[");
  for (size_t i = 0; i < b_length; i++) {
    if (i != 0) {
      ut_string_append(b_value_string, ", ");
    }
    ut_string_append_printf(b_value_string, "%lu", b_value[i]);
  }
  ut_string_append(b_value_string, "]");

  ut_cstring_ref a_value_string = ut_object_to_string(a_value);
  fprintf(stderr,
          "%s:%d List %s doesn't have expected content:\n"
          "  %s\n"
          "  %s\n",
          file, line, a_name, a_value_string,
          ut_string_get_text(b_value_string));

  abort();
}

void _ut_assert_int64_list_equal(const char *file, int line, const char *a_name,
                                 UtObject *a_value, int64_t *b_value,
                                 size_t b_length) {
  if (ut_list_get_length(a_value) == b_length) {
    bool match = true;
    for (size_t i = 0; i < b_length; i++) {
      if (ut_int64_list_get_element(a_value, i) != b_value[i]) {
        match = false;
        break;
      }
    }

    if (match) {
      return;
    }
  }

  UtObjectRef b_value_string = ut_string_new("<int64>[");
  for (size_t i = 0; i < b_length; i++) {
    if (i != 0) {
      ut_string_append(b_value_string, ", ");
    }
    ut_string_append_printf(b_value_string, "%li", b_value[i]);
  }
  ut_string_append(b_value_string, "]");

  ut_cstring_ref a_value_string = ut_object_to_string(a_value);
  fprintf(stderr,
          "%s:%d List %s doesn't have expected content:\n"
          "  %s\n"
          "  %s\n",
          file, line, a_name, a_value_string,
          ut_string_get_text(b_value_string));

  abort();
}

void _ut_assert_float32_list_equal(const char *file, int line,
                                   const char *a_name, UtObject *a_value,
                                   float *b_value, size_t b_length) {
  if (ut_list_get_length(a_value) == b_length) {
    bool match = true;
    for (size_t i = 0; i < b_length; i++) {
      if (ut_float32_list_get_element(a_value, i) != b_value[i]) {
        match = false;
        break;
      }
    }

    if (match) {
      return;
    }
  }

  UtObjectRef b_value_string = ut_string_new("<float32>[");
  for (size_t i = 0; i < b_length; i++) {
    if (i != 0) {
      ut_string_append(b_value_string, ", ");
    }
    ut_string_append_printf(b_value_string, "%f", b_value[i]);
  }
  ut_string_append(b_value_string, "]");

  ut_cstring_ref a_value_string = ut_object_to_string(a_value);
  fprintf(stderr,
          "%s:%d List %s doesn't have expected content:\n"
          "  %s\n"
          "  %s\n",
          file, line, a_name, a_value_string,
          ut_string_get_text(b_value_string));

  abort();
}

void _ut_assert_float64_list_equal(const char *file, int line,
                                   const char *a_name, UtObject *a_value,
                                   double *b_value, size_t b_length) {
  if (ut_list_get_length(a_value) == b_length) {
    bool match = true;
    for (size_t i = 0; i < b_length; i++) {
      if (ut_float64_list_get_element(a_value, i) != b_value[i]) {
        match = false;
        break;
      }
    }

    if (match) {
      return;
    }
  }

  UtObjectRef b_value_string = ut_string_new("<float64>[");
  for (size_t i = 0; i < b_length; i++) {
    if (i != 0) {
      ut_string_append(b_value_string, ", ");
    }
    ut_string_append_printf(b_value_string, "%g", b_value[i]);
  }
  ut_string_append(b_value_string, "]");

  ut_cstring_ref a_value_string = ut_object_to_string(a_value);
  fprintf(stderr,
          "%s:%d List %s doesn't have expected content:\n"
          "  %s\n"
          "  %s\n",
          file, line, a_name, a_value_string,
          ut_string_get_text(b_value_string));

  abort();
}
