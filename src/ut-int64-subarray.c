#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ut-int64-subarray.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *parent;
  size_t parent_length; // Parent length to catch crash if parent changes size.
  size_t start;
  size_t length;
} UtInt64Subarray;

static int64_t *get_data(UtInt64Subarray *self) {
  assert(ut_list_get_length(self->parent) == self->parent_length);
  return ut_int64_list_get_writable_data(self->parent) + self->start;
}

static int64_t ut_int64_subarray_get_element(UtObject *object, size_t index) {
  UtInt64Subarray *self = (UtInt64Subarray *)object;
  int64_t *data = get_data(self);
  return data[index];
}

static const int64_t *ut_int64_subarray_get_const_data(UtObject *object) {
  UtInt64Subarray *self = (UtInt64Subarray *)object;
  return get_data(self);
}

static int64_t *ut_int64_subarray_take_data(UtObject *object) {
  UtInt64Subarray *self = (UtInt64Subarray *)object;
  int64_t *data = get_data(self);
  int64_t *copy = malloc(sizeof(int64_t) * self->length);
  memcpy(copy, data, self->length);
  return copy;
}

static size_t ut_int64_subarray_get_length(UtObject *object) {
  UtInt64Subarray *self = (UtInt64Subarray *)object;
  return self->length;
}

static UtObject *ut_int64_subarray_get_element_object(UtObject *object,
                                                      size_t index) {
  UtInt64Subarray *self = (UtInt64Subarray *)object;
  int64_t *data = get_data(self);
  return ut_int64_new(data[index]);
}

static UtObject *ut_int64_subarray_get_sublist(UtObject *object, size_t start,
                                               size_t count) {
  UtInt64Subarray *self = (UtInt64Subarray *)object;
  return ut_int64_subarray_new(self->parent, self->start + start, count);
}

static UtObject *ut_int64_subarray_copy(UtObject *object) {
  UtInt64Subarray *self = (UtInt64Subarray *)object;
  UtObject *copy = ut_int64_array_new();
  int64_t *data = get_data(self);
  ut_int64_list_append_block(copy, data, self->length);
  return copy;
}

static char *ut_int64_subarray_to_string(UtObject *object) {
  UtInt64Subarray *self = (UtInt64Subarray *)object;
  int64_t *data = get_data(self);
  UtObjectRef string = ut_string_new("<int64>[");
  for (size_t i = 0; i < self->length; i++) {
    if (i != 0) {
      ut_string_append(string, ", ");
    }
    ut_cstring_ref value_string = ut_cstring_new_printf("%lu", data[i]);
    ut_string_append(string, value_string);
  }
  ut_string_append(string, "]");

  return ut_string_take_text(string);
}

static void ut_int64_subarray_cleanup(UtObject *object) {
  UtInt64Subarray *self = (UtInt64Subarray *)object;
  ut_object_unref(self->parent);
}

static UtInt64ListInterface int64_list_interface = {
    .get_element = ut_int64_subarray_get_element,
    .get_data = ut_int64_subarray_get_const_data,
    .take_data = ut_int64_subarray_take_data};

static UtListInterface list_interface = {
    .is_mutable = false,
    .get_length = ut_int64_subarray_get_length,
    .get_element = ut_int64_subarray_get_element_object,
    .get_sublist = ut_int64_subarray_get_sublist,
    .copy = ut_int64_subarray_copy};

static UtObjectInterface object_interface = {
    .type_name = "UtInt64Subarray",
    .to_string = ut_int64_subarray_to_string,
    .cleanup = ut_int64_subarray_cleanup,
    .interfaces = {{&ut_int64_list_id, &int64_list_interface},
                   {&ut_list_id, &list_interface},
                   {NULL, NULL}}};

UtObject *ut_int64_subarray_new(UtObject *parent, size_t start, size_t length) {
  UtObject *object = ut_object_new(sizeof(UtInt64Subarray), &object_interface);
  UtInt64Subarray *self = (UtInt64Subarray *)object;

  assert(parent != NULL && ut_object_is_int64_array(parent));
  size_t parent_length = ut_list_get_length(parent);
  assert(start + length <= parent_length);

  self->parent = ut_object_ref(parent);
  self->parent_length = parent_length;
  self->start = start;
  self->length = length;
  return object;
}

int64_t *ut_int64_subarray_get_data(UtObject *object) {
  assert(ut_object_is_int64_subarray(object));
  UtInt64Subarray *self = (UtInt64Subarray *)object;
  return get_data(self);
}

bool ut_object_is_int64_subarray(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
