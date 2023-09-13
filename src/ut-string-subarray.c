#include <assert.h>

#include "ut-list-private.h"
#include "ut-string-subarray.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *parent;
  size_t parent_length; // Parent length to catch crash if parent changes size.
  size_t start;
  size_t length;
} UtStringSubarray;

static const char *get_element(UtStringSubarray *self, size_t index) {
  assert(ut_list_get_length(self->parent) == self->parent_length);
  return ut_string_list_get_element(self->parent, self->start + index);
}

static const char *ut_string_subarray_get_element(UtObject *object,
                                                  size_t index) {
  UtStringSubarray *self = (UtStringSubarray *)object;
  return get_element(self, index);
}

static char **ut_string_subarray_take_data(UtObject *object) {
  UtStringSubarray *self = (UtStringSubarray *)object;
  char **copy = malloc(sizeof(char *) * (self->length + 1));
  for (size_t i = 0; i < self->length; i++) {
    copy[i] = ut_cstring_new(get_element(self, i));
  }
  copy[self->length] = NULL;
  return copy;
}

static size_t ut_string_subarray_get_length(UtObject *object) {
  UtStringSubarray *self = (UtStringSubarray *)object;
  return self->length;
}

static UtObject *ut_string_subarray_get_element_object(UtObject *object,
                                                       size_t index) {
  UtStringSubarray *self = (UtStringSubarray *)object;
  return ut_string_new(get_element(self, index));
}

static UtObject *ut_string_subarray_get_sublist(UtObject *object, size_t start,
                                                size_t count) {
  UtStringSubarray *self = (UtStringSubarray *)object;
  return ut_string_subarray_new(self->parent, self->start + start, count);
}

static UtObject *ut_string_subarray_copy(UtObject *object) {
  UtStringSubarray *self = (UtStringSubarray *)object;
  UtObject *copy = ut_string_array_new();
  for (size_t i = 0; i < self->length; i++) {
    ut_string_list_append(copy, get_element(self, i));
  }
  return copy;
}

static void ut_string_subarray_cleanup(UtObject *object) {
  UtStringSubarray *self = (UtStringSubarray *)object;
  ut_object_unref(self->parent);
}

static UtStringListInterface string_list_interface = {
    .get_element = ut_string_subarray_get_element,
    .take_data = ut_string_subarray_take_data};

static UtListInterface list_interface = {
    .is_mutable = false,
    .get_length = ut_string_subarray_get_length,
    .get_element = ut_string_subarray_get_element_object,
    .get_sublist = ut_string_subarray_get_sublist,
    .copy = ut_string_subarray_copy};

static UtObjectInterface object_interface = {
    .type_name = "UtStringSubarray",
    .to_string = _ut_list_to_string,
    .cleanup = ut_string_subarray_cleanup,
    .interfaces = {{&ut_string_list_id, &string_list_interface},
                   {&ut_list_id, &list_interface},
                   {NULL, NULL}}};

UtObject *ut_string_subarray_new(UtObject *parent, size_t start,
                                 size_t length) {
  UtObject *object = ut_object_new(sizeof(UtStringSubarray), &object_interface);
  UtStringSubarray *self = (UtStringSubarray *)object;

  assert(parent != NULL && ut_object_is_string_array(parent));
  size_t parent_length = ut_list_get_length(parent);
  assert(start + length <= parent_length);

  self->parent = ut_object_ref(parent);
  self->parent_length = parent_length;
  self->start = start;
  self->length = length;
  return object;
}

bool ut_object_is_string_subarray(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
