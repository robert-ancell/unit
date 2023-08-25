#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ut-uint16-subarray.h"
#include "ut.h"

typedef struct {
  UtObject object;
  uint16_t *data;
  size_t data_length;
} UtUint16Array;

static void resize_list(UtUint16Array *self, size_t length) {
  self->data = realloc(self->data, sizeof(uint16_t) * length);
  for (size_t i = self->data_length; i < length; i++) {
    self->data[i] = 0;
  }
  self->data_length = length;
}

static int get_nibble(char c) {
  if (c >= '0' && c <= '9') {
    return c - '0';
  } else if (c >= 'a' && c <= 'f') {
    return c - 'a' + 10;
  } else if (c >= 'A' && c <= 'F') {
    return c - 'A' + 10;
  } else {
    return -1;
  }
}

static uint16_t ut_uint16_array_get_element(UtObject *object, size_t index) {
  UtUint16Array *self = (UtUint16Array *)object;
  return self->data[index];
}

static const uint16_t *ut_uint16_array_get_const_data(UtObject *object) {
  UtUint16Array *self = (UtUint16Array *)object;
  return self->data;
}

static uint16_t *ut_uint16_array_get_writable_data(UtObject *object) {
  UtUint16Array *self = (UtUint16Array *)object;
  return self->data;
}

static uint16_t *ut_uint16_array_take_data(UtObject *object) {
  UtUint16Array *self = (UtUint16Array *)object;
  uint16_t *result = self->data;
  self->data = NULL;
  self->data_length = 0;
  return result;
}

static void ut_uint16_array_insert(UtObject *object, size_t index,
                                   const uint16_t *data, size_t data_length) {
  assert(ut_object_is_uint16_array(object));
  UtUint16Array *self = (UtUint16Array *)object;

  size_t n_after = self->data_length - index;
  resize_list(self, self->data_length + data_length);

  // Shift existing data up
  for (size_t i = 0; i < n_after; i++) {
    size_t new_index = self->data_length - 1 - i;
    self->data[new_index] = self->data[new_index - data_length];
  }

  // Insert new data
  for (size_t i = 0; i < data_length; i++) {
    self->data[index + i] = data[i];
  }
}

static void ut_uint16_array_insert_object(UtObject *object, size_t index,
                                          UtObject *item) {
  assert(ut_object_is_uint16(item));
  uint16_t value = ut_uint16_get_value(item);
  ut_uint16_array_insert(object, index, &value, 1);
}

static void ut_uint16_array_remove(UtObject *object, size_t index,
                                   size_t count) {
  UtUint16Array *self = (UtUint16Array *)object;
  assert(index <= self->data_length);
  assert(index + count <= self->data_length);
  for (size_t i = index; i < self->data_length - count; i++) {
    self->data[i] = self->data[i + count];
  }
  resize_list(self, self->data_length - count);
}

static void ut_uint16_array_resize(UtObject *object, size_t length) {
  UtUint16Array *self = (UtUint16Array *)object;
  resize_list(self, length);
}

static size_t ut_uint16_array_get_length(UtObject *object) {
  UtUint16Array *self = (UtUint16Array *)object;
  return self->data_length;
}

static UtObject *ut_uint16_array_get_element_object(UtObject *object,
                                                    size_t index) {
  UtUint16Array *self = (UtUint16Array *)object;
  return ut_uint16_new(self->data[index]);
}

static UtObject *ut_uint16_array_get_sublist(UtObject *object, size_t start,
                                             size_t count) {
  return ut_uint16_subarray_new(object, start, count);
}

static UtObject *ut_uint16_array_copy(UtObject *object) {
  UtUint16Array *self = (UtUint16Array *)object;
  UtObject *copy = ut_uint16_array_new();
  ut_uint16_array_insert(copy, 0, self->data, self->data_length);
  return copy;
}

static char *ut_uint16_array_to_string(UtObject *object) {
  UtUint16Array *self = (UtUint16Array *)object;
  UtObjectRef string = ut_string_new("<uint16>[");
  for (size_t i = 0; i < self->data_length; i++) {
    if (i != 0) {
      ut_string_append(string, ", ");
    }
    ut_string_append_printf(string, "%u", self->data[i]);
  }
  ut_string_append(string, "]");

  return ut_string_take_text(string);
}

static bool ut_uint16_array_equal(UtObject *object, UtObject *other) {
  UtUint16Array *self = (UtUint16Array *)object;
  if (!ut_object_implements_uint16_list(other)) {
    return false;
  }
  if (self->data_length != ut_list_get_length(other)) {
    return false;
  }
  for (size_t i = 0; i < self->data_length; i++) {
    if (self->data[i] != ut_uint16_list_get_element(other, i)) {
      return false;
    }
  }
  return true;
}

static void ut_uint16_array_cleanup(UtObject *object) {
  UtUint16Array *self = (UtUint16Array *)object;
  free(self->data);
}

static UtUint16ListInterface uint16_list_interface = {
    .get_element = ut_uint16_array_get_element,
    .get_data = ut_uint16_array_get_const_data,
    .get_writable_data = ut_uint16_array_get_writable_data,
    .take_data = ut_uint16_array_take_data,
    .insert = ut_uint16_array_insert};

static UtListInterface list_interface = {
    .is_mutable = true,
    .get_length = ut_uint16_array_get_length,
    .get_element = ut_uint16_array_get_element_object,
    .get_sublist = ut_uint16_array_get_sublist,
    .copy = ut_uint16_array_copy,
    .insert = ut_uint16_array_insert_object,
    .remove = ut_uint16_array_remove,
    .resize = ut_uint16_array_resize};

static UtObjectInterface object_interface = {
    .type_name = "UtUint16Array",
    .to_string = ut_uint16_array_to_string,
    .equal = ut_uint16_array_equal,
    .cleanup = ut_uint16_array_cleanup,
    .interfaces = {{&ut_uint16_list_id, &uint16_list_interface},
                   {&ut_list_id, &list_interface},
                   {NULL, NULL}}};

UtObject *ut_uint16_array_new() {
  return ut_object_new(sizeof(UtUint16Array), &object_interface);
}

UtObject *ut_uint16_array_new_sized(size_t length) {
  UtObject *object = ut_uint16_array_new();
  UtUint16Array *self = (UtUint16Array *)object;

  self->data = malloc(sizeof(uint16_t) * length);
  self->data_length = length;
  for (size_t i = 0; i < length; i++) {
    self->data[i] = 0;
  }

  return object;
}

UtObject *ut_uint16_array_new_from_elements(size_t length, ...) {
  va_list ap;
  va_start(ap, length);
  UtObject *object = ut_uint16_array_new_from_va_elements(length, ap);
  va_end(ap);
  return object;
}

UtObject *ut_uint16_array_new_from_va_elements(size_t length, va_list ap) {
  UtObject *object = ut_uint16_array_new();
  UtUint16Array *self = (UtUint16Array *)object;

  resize_list(self, length);
  for (size_t i = 0; i < length; i++) {
    self->data[i] = va_arg(ap, int);
  }

  return object;
}

UtObject *ut_uint16_array_new_from_hex_string(const char *hex) {
  size_t hex_length = strlen(hex);
  if (hex_length % 4 != 0) {
    return ut_error_new("Invalid hexadecimal string length");
  }
  size_t length = hex_length / 4;

  UtObjectRef object = ut_uint16_array_new();
  UtUint16Array *self = (UtUint16Array *)object;
  resize_list(self, length);
  for (size_t i = 0; i < length; i++) {
    int nibble0 = get_nibble(hex[i * 4]);
    int nibble1 = get_nibble(hex[(i * 4) + 1]);
    int nibble2 = get_nibble(hex[(i * 4) + 2]);
    int nibble3 = get_nibble(hex[(i * 4) + 3]);
    if (nibble0 < 0 || nibble1 < 0 || nibble2 < 0 || nibble3 < 0) {
      return ut_error_new("Invalid hexadecimal string");
    }
    self->data[i] = nibble0 << 12 | nibble1 << 8 | nibble2 << 4 | nibble3;
  }

  return ut_object_ref(object);
}

bool ut_object_is_uint16_array(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
