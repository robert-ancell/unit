#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ut-uint16-subarray.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *parent;
  size_t parent_length; // Parent length to catch crash if parent changes size.
  size_t start;
  size_t length;
} UtUint16Subarray;

static uint16_t *get_data(UtUint16Subarray *self) {
  assert(ut_list_get_length(self->parent) == self->parent_length);
  return ut_uint16_array_get_data(self->parent) + self->start;
}

static uint16_t ut_uint16_subarray_get_element(UtObject *object, size_t index) {
  UtUint16Subarray *self = (UtUint16Subarray *)object;
  uint16_t *data = get_data(self);
  return data[index];
}

static const uint16_t *ut_uint16_subarray_get_const_data(UtObject *object) {
  UtUint16Subarray *self = (UtUint16Subarray *)object;
  return get_data(self);
}

static uint16_t *ut_uint16_subarray_take_data(UtObject *object) {
  UtUint16Subarray *self = (UtUint16Subarray *)object;
  uint16_t *data = get_data(self);
  uint16_t *copy = malloc(sizeof(uint16_t) * self->length);
  memcpy(copy, data, self->length);
  return copy;
}

static size_t ut_uint16_subarray_get_length(UtObject *object) {
  UtUint16Subarray *self = (UtUint16Subarray *)object;
  return self->length;
}

static UtObject *ut_uint16_subarray_get_element_object(UtObject *object,
                                                       size_t index) {
  UtUint16Subarray *self = (UtUint16Subarray *)object;
  uint16_t *data = get_data(self);
  return ut_uint16_new(data[index]);
}

static UtObject *ut_uint16_subarray_get_sublist(UtObject *object, size_t start,
                                                size_t count) {
  UtUint16Subarray *self = (UtUint16Subarray *)object;
  return ut_uint16_subarray_new(self->parent, self->start + start, count);
}

static UtObject *ut_uint16_subarray_copy(UtObject *object) {
  UtUint16Subarray *self = (UtUint16Subarray *)object;
  UtObject *copy = ut_uint16_array_new();
  uint16_t *data = get_data(self);
  ut_uint16_list_append_block(copy, data, self->length);
  return copy;
}

static char *ut_uint16_subarray_to_string(UtObject *object) {
  UtUint16Subarray *self = (UtUint16Subarray *)object;
  uint16_t *data = get_data(self);
  UtObjectRef string = ut_string_new("<uint16>[");
  for (size_t i = 0; i < self->length; i++) {
    if (i != 0) {
      ut_string_append(string, ", ");
    }
    ut_string_append_printf(string, "%d", data[i]);
  }
  ut_string_append(string, "]");

  return ut_string_take_text(string);
}

static void ut_uint16_subarray_cleanup(UtObject *object) {
  UtUint16Subarray *self = (UtUint16Subarray *)object;
  ut_object_unref(self->parent);
}

static UtUint16ListInterface uint16_list_interface = {
    .get_element = ut_uint16_subarray_get_element,
    .get_data = ut_uint16_subarray_get_const_data,
    .take_data = ut_uint16_subarray_take_data};

static UtListInterface list_interface = {
    .is_mutable = false,
    .get_length = ut_uint16_subarray_get_length,
    .get_element = ut_uint16_subarray_get_element_object,
    .get_sublist = ut_uint16_subarray_get_sublist,
    .copy = ut_uint16_subarray_copy};

static UtObjectInterface object_interface = {
    .type_name = "UtUint16Subarray",
    .to_string = ut_uint16_subarray_to_string,
    .cleanup = ut_uint16_subarray_cleanup,
    .interfaces = {{&ut_uint16_list_id, &uint16_list_interface},
                   {&ut_list_id, &list_interface},
                   {NULL, NULL}}};

UtObject *ut_uint16_subarray_new(UtObject *parent, size_t start,
                                 size_t length) {
  UtObject *object = ut_object_new(sizeof(UtUint16Subarray), &object_interface);
  UtUint16Subarray *self = (UtUint16Subarray *)object;

  assert(parent != NULL && ut_object_is_uint16_array(parent));
  size_t parent_length = ut_list_get_length(parent);
  assert(start + length <= parent_length);

  self->parent = ut_object_ref(parent);
  self->parent_length = parent_length;
  self->start = start;
  self->length = length;
  return object;
}

uint16_t *ut_uint16_subarray_get_data(UtObject *object) {
  assert(ut_object_is_uint16_subarray(object));
  UtUint16Subarray *self = (UtUint16Subarray *)object;
  return get_data(self);
}

bool ut_object_is_uint16_subarray(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
