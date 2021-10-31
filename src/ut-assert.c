#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ut-assert.h"
#include "ut-cstring.h"
#include "ut-error.h"

static char *append_string(char *value, const char *suffix) {
  size_t value_length = value != NULL ? strlen(value) : 0;
  size_t suffix_length = strlen(suffix);

  value = realloc(value, value_length + suffix_length + 1);
  memcpy(value + value_length, suffix, suffix_length + 1);

  return value;
}

static char *escape_character(char *value, char c) {
  size_t value_length = strlen(value);

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
    return strdup("NULL");
  }
  char *escaped = append_string(NULL, "\"");
  for (const char *c = value; *c != '\0'; c++) {
    escaped = escape_character(escaped, *c);
  }
  return append_string(escaped, "\"");
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

  ut_cstring a_value_string =
      a_value != NULL ? ut_object_to_string(a_value) : strdup("NULL");
  ut_cstring b_value_string =
      b_value != NULL ? ut_object_to_string(b_value) : strdup("NULL");
  fprintf(stderr,
          "%s:%d Strings %s and %s are not equal:\n"
          "  %s\n"
          "  %s\n",
          file, line, a_name, b_name, a_value_string, b_value_string);

  abort();
}

void _ut_assert_is_error(const char *file, int line, const char *name,
                         UtObject *value) {
  if (ut_object_implements_error(value)) {
    return;
  }

  ut_cstring value_string = ut_object_to_string(value);
  fprintf(stderr,
          "%s:%d Object %s is not an error:\n"
          "  %s\n",
          file, line, name, value_string);

  abort();
}

void _ut_assert_is_not_error(const char *file, int line, const char *name,
                             UtObject *value) {
  if (!ut_object_implements_error(value)) {
    return;
  }

  ut_cstring value_string = ut_object_to_string(value);
  fprintf(stderr,
          "%s:%d Object %s is an error:\n"
          "  %s\n",
          file, line, name, value_string);

  abort();
}

void _ut_assert_cstring_equal(const char *file, int line, const char *a_name,
                              const char *a_value, const char *b_name,
                              const char *b_value) {
  if (a_value == NULL && b_value == NULL) {
    return;
  }
  if (a_value != NULL && b_value != NULL && strcmp(a_value, b_value) == 0) {
    return;
  }

  ut_cstring escaped_a_value = escape_string(a_value);
  ut_cstring escaped_b_value = escape_string(b_value);
  fprintf(stderr,
          "%s:%d Strings %s and %s are not equal:\n"
          "  %s\n"
          "  %s\n",
          file, line, a_name, b_name, escaped_a_value, escaped_b_value);

  abort();
}