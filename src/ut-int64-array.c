#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "ut-int64-subarray.h"
#include "ut.h"

typedef struct {
  UtObject object;
  int64_t *data;
  size_t data_length;
} UtInt64Array;

static void resize_list(UtInt64Array *self, size_t length) {
  self->data = realloc(self->data, sizeof(int64_t) * length);
  for (size_t i = self->data_length; i < length; i++) {
    self->data[i] = 0;
  }
  self->data_length = length;
}

static int64_t ut_int64_array_get_element(UtObject *object, size_t index) {
  UtInt64Array *self = (UtInt64Array *)object;
  return self->data[index];
}

static const int64_t *ut_int64_array_get_const_data(UtObject *object) {
  UtInt64Array *self = (UtInt64Array *)object;
  return self->data;
}

static int64_t *ut_int64_array_get_writable_data(UtObject *object) {
  UtInt64Array *self = (UtInt64Array *)object;
  return self->data;
}

static int64_t *ut_int64_array_take_data(UtObject *object) {
  UtInt64Array *self = (UtInt64Array *)object;
  int64_t *result = self->data;
  self->data = NULL;
  self->data_length = 0;
  return result;
}

static void ut_int64_array_insert(UtObject *object, size_t index,
                                  const int64_t *data, size_t data_length) {
  UtInt64Array *self = (UtInt64Array *)object;

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

static void ut_int64_array_insert_object(UtObject *object, size_t index,
                                         UtObject *item) {
  assert(ut_object_is_int64(item));
  int64_t value = ut_int64_get_value(item);
  ut_int64_array_insert(object, index, &value, 1);
}

static void ut_int64_array_remove(UtObject *object, size_t index,
                                  size_t count) {
  UtInt64Array *self = (UtInt64Array *)object;
  assert(index <= self->data_length);
  assert(index + count <= self->data_length);
  for (size_t i = index; i < self->data_length - count; i++) {
    self->data[i] = self->data[i + count];
  }
  resize_list(self, self->data_length - count);
}

static void ut_int64_array_resize(UtObject *object, size_t length) {
  UtInt64Array *self = (UtInt64Array *)object;
  resize_list(self, length);
}

static size_t ut_int64_array_get_length(UtObject *object) {
  UtInt64Array *self = (UtInt64Array *)object;
  return self->data_length;
}

static UtObject *ut_int64_array_get_element_object(UtObject *object,
                                                   size_t index) {
  UtInt64Array *self = (UtInt64Array *)object;
  return ut_int64_new(self->data[index]);
}

static UtObject *ut_int64_array_get_sublist(UtObject *object, size_t start,
                                            size_t count) {
  return ut_int64_subarray_new(object, start, count);
}

static UtObject *ut_int64_array_copy(UtObject *object) {
  UtInt64Array *self = (UtInt64Array *)object;
  UtObject *copy = ut_int64_array_new();
  ut_int64_array_insert(copy, 0, self->data, self->data_length);
  return copy;
}

static char *ut_int64_array_to_string(UtObject *object) {
  UtInt64Array *self = (UtInt64Array *)object;
  UtObjectRef string = ut_string_new("<int64>[");
  for (size_t i = 0; i < self->data_length; i++) {
    if (i != 0) {
      ut_string_append(string, ", ");
    }
    ut_cstring_ref value_string = ut_cstring_new_printf("%lu", self->data[i]);
    ut_string_append(string, value_string);
  }
  ut_string_append(string, "]");

  return ut_string_take_text(string);
}

static bool ut_int64_array_equal(UtObject *object, UtObject *other) {
  UtInt64Array *self = (UtInt64Array *)object;
  if (!ut_object_implements_int64_list(other)) {
    return false;
  }
  if (self->data_length != ut_list_get_length(other)) {
    return false;
  }
  for (size_t i = 0; i < self->data_length; i++) {
    if (self->data[i] != ut_int64_list_get_element(other, i)) {
      return false;
    }
  }
  return true;
}

static void ut_int64_array_cleanup(UtObject *object) {
  UtInt64Array *self = (UtInt64Array *)object;
  free(self->data);
}

static UtInt64ListInterface int64_list_interface = {
    .get_element = ut_int64_array_get_element,
    .get_data = ut_int64_array_get_const_data,
    .get_writable_data = ut_int64_array_get_writable_data,
    .take_data = ut_int64_array_take_data,
    .insert = ut_int64_array_insert};

static UtListInterface list_interface = {
    .is_mutable = true,
    .get_length = ut_int64_array_get_length,
    .get_element = ut_int64_array_get_element_object,
    .get_sublist = ut_int64_array_get_sublist,
    .copy = ut_int64_array_copy,
    .insert = ut_int64_array_insert_object,
    .remove = ut_int64_array_remove,
    .resize = ut_int64_array_resize};

static UtObjectInterface object_interface = {
    .type_name = "UtInt64Array",
    .to_string = ut_int64_array_to_string,
    .equal = ut_int64_array_equal,
    .cleanup = ut_int64_array_cleanup,
    .interfaces = {{&ut_int64_list_id, &int64_list_interface},
                   {&ut_list_id, &list_interface},
                   {NULL, NULL}}};

UtObject *ut_int64_array_new() {
  return ut_object_new(sizeof(UtInt64Array), &object_interface);
}

UtObject *ut_int64_array_new_sized(size_t length) {
  UtObject *object = ut_int64_array_new();
  UtInt64Array *self = (UtInt64Array *)object;

  self->data = malloc(sizeof(int64_t) * length);
  self->data_length = length;
  for (size_t i = 0; i < length; i++) {
    self->data[i] = 0;
  }

  return object;
}

UtObject *ut_int64_array_new_from_elements(size_t length, ...) {
  va_list ap;
  va_start(ap, length);
  UtObject *object = ut_int64_array_new_from_va_elements(length, ap);
  va_end(ap);
  return object;
}

UtObject *ut_int64_array_new_from_va_elements(size_t length, va_list ap) {
  UtObject *object = ut_int64_array_new();
  UtInt64Array *self = (UtInt64Array *)object;

  resize_list(self, length);
  for (size_t i = 0; i < length; i++) {
    self->data[i] = va_arg(ap, long long int);
  }

  return object;
}

int64_t *ut_int64_array_get_data(UtObject *object) {
  assert(ut_object_is_int64_array(object));
  UtInt64Array *self = (UtInt64Array *)object;
  return self->data;
}

bool ut_object_is_int64_array(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
