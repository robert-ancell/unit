#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "ut.h"

typedef struct {
  UtObject object;
  bool *data;
  size_t data_length;
} UtBooleanArray;

static void resize_list(UtBooleanArray *self, size_t length) {
  self->data = realloc(self->data, sizeof(bool) * length);
  for (size_t i = self->data_length; i < length; i++) {
    self->data[i] = 0;
  }
  self->data_length = length;
}

static bool ut_boolean_array_get_element(UtObject *object, size_t index) {
  UtBooleanArray *self = (UtBooleanArray *)object;
  return self->data[index];
}

static void ut_boolean_array_insert(UtObject *object, size_t index, bool item) {
  UtBooleanArray *self = (UtBooleanArray *)object;

  resize_list(self, self->data_length + 1);

  // Shift existing data up
  size_t n_after = self->data_length - index;
  for (size_t i = 0; i < n_after; i++) {
    size_t new_index = self->data_length - 1 - i;
    self->data[new_index] = self->data[new_index - 1];
  }

  // Insert new data
  self->data[index] = item;
}

static void ut_boolean_array_insert_object(UtObject *object, size_t index,
                                           UtObject *item) {
  assert(ut_object_is_boolean(item));
  bool value = ut_boolean_get_value(item);
  ut_boolean_array_insert(object, index, value);
}

static void ut_boolean_array_remove(UtObject *object, size_t index,
                                    size_t count) {
  UtBooleanArray *self = (UtBooleanArray *)object;
  assert(index <= self->data_length);
  assert(index + count <= self->data_length);
  for (size_t i = index; i < self->data_length - count; i++) {
    self->data[i] = self->data[i + count];
  }
  resize_list(self, self->data_length - count);
}

static void ut_boolean_array_resize(UtObject *object, size_t length) {
  UtBooleanArray *self = (UtBooleanArray *)object;
  resize_list(self, length);
}

static size_t ut_boolean_array_get_length(UtObject *object) {
  UtBooleanArray *self = (UtBooleanArray *)object;
  return self->data_length;
}

static UtObject *ut_boolean_array_get_element_object(UtObject *object,
                                                     size_t index) {
  UtBooleanArray *self = (UtBooleanArray *)object;
  return ut_boolean_new(self->data[index]);
}

static char *ut_boolean_array_to_string(UtObject *object) {
  UtBooleanArray *self = (UtBooleanArray *)object;
  UtObjectRef string = ut_string_new("<bool>[");
  for (size_t i = 0; i < self->data_length; i++) {
    if (i != 0) {
      ut_string_append(string, ", ");
    }
    ut_string_append_printf(string, "%s", self->data[i] ? "true" : "false");
  }
  ut_string_append(string, "]");

  return ut_string_take_text(string);
}

static bool ut_boolean_array_equal(UtObject *object, UtObject *other) {
  UtBooleanArray *self = (UtBooleanArray *)object;
  if (!ut_object_implements_boolean_list(other)) {
    return false;
  }
  if (self->data_length != ut_list_get_length(other)) {
    return false;
  }
  for (size_t i = 0; i < self->data_length; i++) {
    if (self->data[i] != ut_boolean_list_get_element(other, i)) {
      return false;
    }
  }
  return true;
}

static void ut_boolean_array_cleanup(UtObject *object) {
  UtBooleanArray *self = (UtBooleanArray *)object;
  free(self->data);
}

static UtBooleanListInterface boolean_list_interface = {
    .get_element = ut_boolean_array_get_element,
    .insert = ut_boolean_array_insert};

static UtListInterface list_interface = {
    .is_mutable = true,
    .get_length = ut_boolean_array_get_length,
    .get_element = ut_boolean_array_get_element_object,
    .insert = ut_boolean_array_insert_object,
    .remove = ut_boolean_array_remove,
    .resize = ut_boolean_array_resize};

static UtObjectInterface object_interface = {
    .type_name = "UtBooleanArray",
    .to_string = ut_boolean_array_to_string,
    .equal = ut_boolean_array_equal,
    .cleanup = ut_boolean_array_cleanup,
    .interfaces = {{&ut_boolean_list_id, &boolean_list_interface},
                   {&ut_list_id, &list_interface},
                   {NULL, NULL}}};

UtObject *ut_boolean_array_new() {
  return ut_object_new(sizeof(UtBooleanArray), &object_interface);
}

UtObject *ut_boolean_array_new_sized(size_t length) {
  UtObject *object = ut_boolean_array_new();
  UtBooleanArray *self = (UtBooleanArray *)object;

  self->data = malloc(sizeof(bool) * length);
  self->data_length = length;
  for (size_t i = 0; i < length; i++) {
    self->data[i] = 0;
  }

  return object;
}

bool *ut_boolean_array_get_data(UtObject *object) {
  assert(ut_object_is_boolean_array(object));
  UtBooleanArray *self = (UtBooleanArray *)object;
  return self->data;
}

bool ut_object_is_boolean_array(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
