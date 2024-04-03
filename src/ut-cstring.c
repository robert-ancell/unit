#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "ut.h"

char *ut_cstring_new(const char *value) { return strdup(value); }

char *ut_cstring_new_substring(const char *value, size_t start, size_t end) {
  assert(end >= start);
  return strndup(value + start, end - start);
}

char *ut_cstring_new_sized(const char *value, size_t length) {
  return strndup(value, length);
}

char *ut_cstring_new_printf(const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  char *result = ut_cstring_new_vprintf(format, ap);
  va_end(ap);

  return result;
}

char *ut_cstring_new_vprintf(const char *format, va_list ap) {
  char empty = '\0';
  va_list ap2;
  va_copy(ap2, ap);
  int length = vsnprintf(&empty, 1, format, ap2);
  va_end(ap2);
  char *result = malloc(sizeof(char) * (length + 1));
  vsnprintf(result, length + 1, format, ap);
  return result;
}

char *ut_cstring_new_lowercase(const char *value) {
  char *result = strdup(value);
  for (char *c = result; *c != '\0'; c++) {
    if (*c >= 'A' && *c <= 'Z') {
      *c += 32;
    }
  }

  return result;
}

char *ut_cstring_new_uppercase(const char *value) {
  char *result = strdup(value);
  for (char *c = result; *c != '\0'; c++) {
    if (*c >= 'a' && *c <= 'z') {
      *c -= 32;
    }
  }

  return result;
}

char *ut_cstring_new_join(const char *separator, const char *word0, ...) {
  if (separator == NULL) {
    separator = "";
  }
  if (word0 == NULL) {
    return ut_cstring_new("");
  }

  size_t separator_length = ut_cstring_get_length(separator);
  size_t word0_length = ut_cstring_get_length(word0);

  size_t word_length = 1;
  size_t result_length = word0_length;
  va_list ap;

  va_list ap2;
  va_copy(ap2, ap);
  va_start(ap2, word0);
  while (true) {
    const char *v = va_arg(ap2, const char *);
    if (v == NULL) {
      break;
    }
    word_length++;
    result_length += ut_cstring_get_length(v);
  }
  va_end(ap2);

  result_length += (word_length - 1) * ut_cstring_get_length(separator);
  char *result = malloc(sizeof(char) * (result_length + 1));
  memcpy(result, word0, word0_length);
  size_t offset = word0_length;
  va_start(ap, word0);
  for (size_t i = 1; i < word_length; i++) {
    memcpy(result + offset, separator, separator_length);
    offset += separator_length;
    const char *v = va_arg(ap, const char *);
    size_t v_length = ut_cstring_get_length(v);
    memcpy(result + offset, v, v_length);
    offset += v_length;
  }
  va_end(ap);
  result[offset] = '\0';

  return result;
}

void ut_cstring_set(char **string, const char *value) {
  if (*string != NULL) {
    free(*string);
  }
  *string = value != NULL ? ut_cstring_new(value) : NULL;
}

void ut_cstring_clear(char **string) {
  if (*string != NULL) {
    free(*string);
    *string = NULL;
  }
}

char *ut_cstring_take(char **string) {
  char *result = *string;
  *string = NULL;
  return result;
}

size_t ut_cstring_get_length(const char *value) { return strlen(value); }

bool ut_cstring_equal(const char *value1, const char *value2) {
  size_t i = 0;
  while (value1[i] == value2[i] && value1[i] != '\0') {
    i++;
  }
  return value1[i] == '\0' && value2[i] == '\0';
}

bool ut_cstring_starts_with(const char *value, const char *prefix) {
  assert(value != NULL);
  assert(prefix != NULL);

  size_t i = 0;
  while (value[i] != '\0' && prefix[i] != '\0' && value[i] == prefix[i]) {
    i++;
  }
  return prefix[i] == '\0';
}

bool ut_cstring_ends_with(const char *value, const char *suffix) {
  assert(value != NULL);
  assert(suffix != NULL);

  size_t value_length = ut_cstring_get_length(value);
  size_t suffix_length = ut_cstring_get_length(suffix);
  if (suffix_length > value_length) {
    return false;
  }
  return ut_cstring_starts_with(value + value_length - suffix_length, suffix);
}

UtObject *ut_cstring_split(const char *value, const char *separator) {
  size_t separator_length = ut_cstring_get_length(separator);
  UtObject *result = ut_string_list_new();

  if (*value == '\0') {
    return result;
  }

  const char *c = value;
  while (true) {
    size_t length = 0;
    while (*(c + length) != '\0' &&
           !ut_cstring_starts_with(c + length, separator)) {
      length++;
    }
    ut_string_list_append_printf(result, "%.*s", (int)length, c);
    c += length;
    if (*c == '\0') {
      return result;
    }
    c += separator_length;
  }
}
