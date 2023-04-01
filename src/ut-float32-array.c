#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "ut.h"

typedef struct {
  UtObject object;
  float *data;
  size_t data_length;
} UtFloat32Array;

static void resize_list(UtFloat32Array *self, size_t length) {
  self->data = realloc(self->data, sizeof(float) * length);
  for (size_t i = self->data_length; i < length; i++) {
    self->data[i] = 0;
  }
  self->data_length = length;
}

static float ut_float32_array_get_element(UtObject *object, size_t index) {
  UtFloat32Array *self = (UtFloat32Array *)object;
  return self->data[index];
}

static const float *ut_float32_array_get_const_data(UtObject *object) {
  UtFloat32Array *self = (UtFloat32Array *)object;
  return self->data;
}

static float *ut_float32_array_get_writable_data(UtObject *object) {
  UtFloat32Array *self = (UtFloat32Array *)object;
  return self->data;
}

static float *ut_float32_array_take_data(UtObject *object) {
  UtFloat32Array *self = (UtFloat32Array *)object;
  float *result = self->data;
  self->data = NULL;
  self->data_length = 0;
  return result;
}

static void ut_float32_array_insert(UtObject *object, size_t index,
                                    const float *data, size_t data_length) {
  UtFloat32Array *self = (UtFloat32Array *)object;

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

static void ut_float32_array_insert_object(UtObject *object, size_t index,
                                           UtObject *item) {
  assert(ut_object_is_float32(item));
  float value = ut_float32_get_value(item);
  ut_float32_array_insert(object, index, &value, 1);
}

static void ut_float32_array_remove(UtObject *object, size_t index,
                                    size_t count) {
  UtFloat32Array *self = (UtFloat32Array *)object;
  assert(index <= self->data_length);
  assert(index + count <= self->data_length);
  for (size_t i = index; i < self->data_length - count; i++) {
    self->data[i] = self->data[i + count];
  }
  resize_list(self, self->data_length - count);
}

static void ut_float32_array_resize(UtObject *object, size_t length) {
  UtFloat32Array *self = (UtFloat32Array *)object;
  resize_list(self, length);
}

static size_t ut_float32_array_get_length(UtObject *object) {
  UtFloat32Array *self = (UtFloat32Array *)object;
  return self->data_length;
}

static UtObject *ut_float32_array_get_element_object(UtObject *object,
                                                     size_t index) {
  UtFloat32Array *self = (UtFloat32Array *)object;
  return ut_float32_new(self->data[index]);
}

static UtObject *ut_float32_array_copy(UtObject *object) {
  UtFloat32Array *self = (UtFloat32Array *)object;
  UtObject *copy = ut_float32_array_new();
  ut_float32_array_insert(copy, 0, self->data, self->data_length);
  return copy;
}

static char *ut_float32_array_to_string(UtObject *object) {
  UtFloat32Array *self = (UtFloat32Array *)object;
  UtObjectRef string = ut_string_new("<float32>[");
  for (size_t i = 0; i < self->data_length; i++) {
    if (i != 0) {
      ut_string_append(string, ", ");
    }
    ut_string_append_printf(string, "%g", self->data[i]);
  }
  ut_string_append(string, "]");

  return ut_string_take_text(string);
}

static void ut_float32_array_cleanup(UtObject *object) {
  UtFloat32Array *self = (UtFloat32Array *)object;
  free(self->data);
}

static UtFloat32ListInterface float32_list_interface = {
    .get_element = ut_float32_array_get_element,
    .get_data = ut_float32_array_get_const_data,
    .get_writable_data = ut_float32_array_get_writable_data,
    .take_data = ut_float32_array_take_data,
    .insert = ut_float32_array_insert};

static UtListInterface list_interface = {
    .is_mutable = true,
    .get_length = ut_float32_array_get_length,
    .get_element = ut_float32_array_get_element_object,
    .copy = ut_float32_array_copy,
    .insert = ut_float32_array_insert_object,
    .remove = ut_float32_array_remove,
    .resize = ut_float32_array_resize};

static UtObjectInterface object_interface = {
    .type_name = "UtFloat32Array",
    .to_string = ut_float32_array_to_string,
    .cleanup = ut_float32_array_cleanup,
    .interfaces = {{&ut_float32_list_id, &float32_list_interface},
                   {&ut_list_id, &list_interface},
                   {NULL, NULL}}};

UtObject *ut_float32_array_new() {
  return ut_object_new(sizeof(UtFloat32Array), &object_interface);
}

UtObject *ut_float32_array_new_sized(size_t length) {
  UtObject *object = ut_float32_array_new();
  UtFloat32Array *self = (UtFloat32Array *)object;

  self->data = malloc(sizeof(float) * length);
  self->data_length = length;
  for (size_t i = 0; i < length; i++) {
    self->data[i] = 0;
  }

  return object;
}

UtObject *ut_float32_array_new_from_elements(size_t length, ...) {
  va_list ap;
  va_start(ap, length);
  UtObject *object = ut_float32_array_new_from_va_elements(length, ap);
  va_end(ap);
  return object;
}

UtObject *ut_float32_array_new_from_va_elements(size_t length, va_list ap) {
  UtObject *object = ut_float32_array_new();
  UtFloat32Array *self = (UtFloat32Array *)object;

  resize_list(self, length);
  for (size_t i = 0; i < length; i++) {
    self->data[i] = va_arg(ap, double);
  }

  return object;
}

float *ut_float32_array_get_data(UtObject *object) {
  assert(ut_object_is_float32_array(object));
  UtFloat32Array *self = (UtFloat32Array *)object;
  return self->data;
}

bool ut_object_is_float32_array(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
