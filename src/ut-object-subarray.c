#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>

#include "ut-list-private.h"
#include "ut-object-subarray.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *parent;
  size_t parent_length; // Parent length to catch crash if parent changes size.
  size_t start;
  size_t length;
} UtObjectSubarray;

static UtObject *get_element(UtObjectSubarray *self, size_t index) {
  assert(ut_list_get_length(self->parent) == self->parent_length);
  return ut_object_list_get_element(self->parent, self->start + index);
}

static UtObject *ut_object_subarray_get_element(UtObject *object,
                                                size_t index) {
  UtObjectSubarray *self = (UtObjectSubarray *)object;
  return get_element(self, index);
}

static size_t ut_object_subarray_get_length(UtObject *object) {
  UtObjectSubarray *self = (UtObjectSubarray *)object;
  return self->length;
}

static UtObject *ut_object_subarray_get_element_ref(UtObject *object,
                                                    size_t index) {
  UtObjectSubarray *self = (UtObjectSubarray *)object;
  return ut_object_ref(get_element(self, index));
}

static UtObject *ut_object_subarray_get_sublist(UtObject *object, size_t start,
                                                size_t count) {
  UtObjectSubarray *self = (UtObjectSubarray *)object;
  return ut_object_subarray_new(self->parent, self->start + start, count);
}

static UtObject *ut_object_subarray_copy(UtObject *object) {
  UtObjectSubarray *self = (UtObjectSubarray *)object;
  assert(ut_list_get_length(self->parent) == self->parent_length);
  UtObject *copy = ut_object_array_new();
  for (size_t i = 0; i < self->length; i++) {
    ut_list_append(copy, get_element(self, i));
  }
  return (UtObject *)copy;
}

static void ut_object_subarray_cleanup(UtObject *object) {
  UtObjectSubarray *self = (UtObjectSubarray *)object;
  ut_object_unref(self->parent);
}

static UtObjectListInterface object_list_interface = {
    .get_element = ut_object_subarray_get_element};

static UtListInterface list_interface = {
    .is_mutable = false,
    .get_length = ut_object_subarray_get_length,
    .get_element = ut_object_subarray_get_element_ref,
    .get_sublist = ut_object_subarray_get_sublist,
    .copy = ut_object_subarray_copy};

static UtObjectInterface object_interface = {
    .type_name = "UtObjectSubarray",
    .to_string = _ut_list_to_string,
    .cleanup = ut_object_subarray_cleanup,
    .interfaces = {{&ut_object_list_id, &object_list_interface},
                   {&ut_list_id, &list_interface},
                   {NULL, NULL}}};

UtObject *ut_object_subarray_new(UtObject *parent, size_t start,
                                 size_t length) {
  UtObject *object = ut_object_new(sizeof(UtObjectSubarray), &object_interface);
  UtObjectSubarray *self = (UtObjectSubarray *)object;

  assert(parent != NULL && ut_object_is_object_array(parent));
  size_t parent_length = ut_list_get_length(parent);
  assert(start + length <= parent_length);

  self->parent = ut_object_ref(parent);
  self->parent_length = parent_length;
  self->start = start;
  self->length = length;
  return object;
}

bool ut_object_is_object_subarray(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
