#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "ut-int16-subarray.h"
#include "ut.h"

typedef struct {
  UtObject object;
  int16_t *data;
  size_t data_length;
} UtInt16Array;

static void resize_list(UtInt16Array *self, size_t length) {
  self->data = realloc(self->data, sizeof(int16_t) * length);
  for (size_t i = self->data_length; i < length; i++) {
    self->data[i] = 0;
  }
  self->data_length = length;
}

static int16_t ut_int16_array_get_element(UtObject *object, size_t index) {
  UtInt16Array *self = (UtInt16Array *)object;
  return self->data[index];
}

static const int16_t *ut_int16_array_get_const_data(UtObject *object) {
  UtInt16Array *self = (UtInt16Array *)object;
  return self->data;
}

static int16_t *ut_int16_array_get_writable_data(UtObject *object) {
  UtInt16Array *self = (UtInt16Array *)object;
  return self->data;
}

static int16_t *ut_int16_array_take_data(UtObject *object) {
  UtInt16Array *self = (UtInt16Array *)object;
  int16_t *result = self->data;
  self->data = NULL;
  self->data_length = 0;
  return result;
}

static void ut_int16_array_insert(UtObject *object, size_t index,
                                  const int16_t *data, size_t data_length) {
  assert(ut_object_is_int16_array(object));
  UtInt16Array *self = (UtInt16Array *)object;

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

static void ut_int16_array_insert_object(UtObject *object, size_t index,
                                         UtObject *item) {
  assert(ut_object_is_int16(item));
  int16_t value = ut_int16_get_value(item);
  ut_int16_array_insert(object, index, &value, 1);
}

static void ut_int16_array_remove(UtObject *object, size_t index,
                                  size_t count) {
  UtInt16Array *self = (UtInt16Array *)object;
  assert(index <= self->data_length);
  assert(index + count <= self->data_length);
  for (size_t i = index; i < self->data_length - count; i++) {
    self->data[i] = self->data[i + count];
  }
  resize_list(self, self->data_length - count);
}

static void ut_int16_array_resize(UtObject *object, size_t length) {
  UtInt16Array *self = (UtInt16Array *)object;
  resize_list(self, length);
}

static size_t ut_int16_array_get_length(UtObject *object) {
  UtInt16Array *self = (UtInt16Array *)object;
  return self->data_length;
}

static UtObject *ut_int16_array_get_element_object(UtObject *object,
                                                   size_t index) {
  UtInt16Array *self = (UtInt16Array *)object;
  return ut_int16_new(self->data[index]);
}

static UtObject *ut_int16_array_get_sublist(UtObject *object, size_t start,
                                            size_t count) {
  return ut_int16_subarray_new(object, start, count);
}

static UtObject *ut_int16_array_copy(UtObject *object) {
  UtInt16Array *self = (UtInt16Array *)object;
  UtObject *copy = ut_int16_array_new();
  ut_int16_array_insert(copy, 0, self->data, self->data_length);
  return copy;
}

static char *ut_int16_array_to_string(UtObject *object) {
  UtInt16Array *self = (UtInt16Array *)object;
  UtObjectRef string = ut_string_new("<int16>[");
  for (size_t i = 0; i < self->data_length; i++) {
    if (i != 0) {
      ut_string_append(string, ", ");
    }
    ut_string_append_printf(string, "%d", self->data[i]);
  }
  ut_string_append(string, "]");

  return ut_string_take_text(string);
}

static bool ut_int16_array_equal(UtObject *object, UtObject *other) {
  UtInt16Array *self = (UtInt16Array *)object;
  if (!ut_object_implements_int16_list(other)) {
    return false;
  }
  if (self->data_length != ut_list_get_length(other)) {
    return false;
  }
  for (size_t i = 0; i < self->data_length; i++) {
    if (self->data[i] != ut_int16_list_get_element(other, i)) {
      return false;
    }
  }
  return true;
}

static void ut_int16_array_cleanup(UtObject *object) {
  UtInt16Array *self = (UtInt16Array *)object;
  free(self->data);
}

static UtInt16ListInterface int16_list_interface = {
    .get_element = ut_int16_array_get_element,
    .get_data = ut_int16_array_get_const_data,
    .get_writable_data = ut_int16_array_get_writable_data,
    .take_data = ut_int16_array_take_data,
    .insert = ut_int16_array_insert};

static UtListInterface list_interface = {
    .is_mutable = true,
    .get_length = ut_int16_array_get_length,
    .get_element = ut_int16_array_get_element_object,
    .get_sublist = ut_int16_array_get_sublist,
    .copy = ut_int16_array_copy,
    .insert = ut_int16_array_insert_object,
    .remove = ut_int16_array_remove,
    .resize = ut_int16_array_resize};

static UtObjectInterface object_interface = {
    .type_name = "UtInt16Array",
    .to_string = ut_int16_array_to_string,
    .equal = ut_int16_array_equal,
    .cleanup = ut_int16_array_cleanup,
    .interfaces = {{&ut_int16_list_id, &int16_list_interface},
                   {&ut_list_id, &list_interface},
                   {NULL, NULL}}};

UtObject *ut_int16_array_new() {
  return ut_object_new(sizeof(UtInt16Array), &object_interface);
}

UtObject *ut_int16_array_new_sized(size_t length) {
  UtObject *object = ut_int16_array_new();
  UtInt16Array *self = (UtInt16Array *)object;

  self->data = malloc(sizeof(int16_t) * length);
  self->data_length = length;
  for (size_t i = 0; i < length; i++) {
    self->data[i] = 0;
  }

  return object;
}

UtObject *ut_int16_array_new_from_elements(size_t length, ...) {
  va_list ap;
  va_start(ap, length);
  UtObject *object = ut_int16_array_new_from_va_elements(length, ap);
  va_end(ap);
  return object;
}

UtObject *ut_int16_array_new_from_va_elements(size_t length, va_list ap) {
  UtObject *object = ut_int16_array_new();
  UtInt16Array *self = (UtInt16Array *)object;

  resize_list(self, length);
  for (size_t i = 0; i < length; i++) {
    self->data[i] = va_arg(ap, int);
  }

  return object;
}

int16_t *ut_int16_array_get_data(UtObject *object) {
  assert(ut_object_is_int16_array(object));
  UtInt16Array *self = (UtInt16Array *)object;
  return self->data;
}

bool ut_object_is_int16_array(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
