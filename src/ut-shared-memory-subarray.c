#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ut-shared-memory-subarray.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *parent;
  size_t parent_length; // Parent length to catch crash if parent changes size.
  size_t start;
  size_t length;
} UtSharedMemorySubarray;

static uint8_t *get_data(UtSharedMemorySubarray *self) {
  assert(ut_list_get_length(self->parent) == self->parent_length);
  return ut_shared_memory_array_get_data(self->parent) + self->start;
}

static uint8_t ut_shared_memory_subarray_get_element(UtObject *object,
                                                     size_t index) {
  UtSharedMemorySubarray *self = (UtSharedMemorySubarray *)object;
  uint8_t *data = get_data(self);
  return data[index];
}

static const uint8_t *
ut_shared_memory_subarray_get_const_data(UtObject *object) {
  UtSharedMemorySubarray *self = (UtSharedMemorySubarray *)object;
  return get_data(self);
}

static uint8_t *ut_shared_memory_subarray_take_data(UtObject *object) {
  UtSharedMemorySubarray *self = (UtSharedMemorySubarray *)object;
  uint8_t *data = get_data(self);
  uint8_t *copy = malloc(sizeof(uint8_t) * self->length);
  memcpy(copy, data, self->length);
  return copy;
}

static size_t ut_shared_memory_subarray_get_length(UtObject *object) {
  UtSharedMemorySubarray *self = (UtSharedMemorySubarray *)object;
  return self->length;
}

static UtObject *ut_shared_memory_subarray_get_element_object(UtObject *object,
                                                              size_t index) {
  UtSharedMemorySubarray *self = (UtSharedMemorySubarray *)object;
  uint8_t *data = get_data(self);
  return ut_uint8_new(data[index]);
}

static UtObject *ut_shared_memory_subarray_get_sublist(UtObject *object,
                                                       size_t start,
                                                       size_t count) {
  UtSharedMemorySubarray *self = (UtSharedMemorySubarray *)object;
  return ut_shared_memory_subarray_new(self->parent, self->start + start,
                                       count);
}

static UtObject *ut_shared_memory_subarray_copy(UtObject *object) {
  UtSharedMemorySubarray *self = (UtSharedMemorySubarray *)object;
  UtObject *copy = ut_uint8_array_new();
  uint8_t *data = get_data(self);
  ut_uint8_list_append_block(copy, data, self->length);
  return copy;
}

static char *ut_shared_memory_subarray_to_string(UtObject *object) {
  UtSharedMemorySubarray *self = (UtSharedMemorySubarray *)object;
  return ut_cstring_new_printf("<UtSharedMemorySubarray>(length: %zi)",
                               self->length);
}

static void ut_shared_memory_subarray_cleanup(UtObject *object) {
  UtSharedMemorySubarray *self = (UtSharedMemorySubarray *)object;
  ut_object_unref(self->parent);
}

static UtUint8ListInterface uint8_list_interface = {
    .get_element = ut_shared_memory_subarray_get_element,
    .get_data = ut_shared_memory_subarray_get_const_data,
    .take_data = ut_shared_memory_subarray_take_data};

static UtListInterface list_interface = {
    .is_mutable = false,
    .get_length = ut_shared_memory_subarray_get_length,
    .get_element = ut_shared_memory_subarray_get_element_object,
    .get_sublist = ut_shared_memory_subarray_get_sublist,
    .copy = ut_shared_memory_subarray_copy};

static UtObjectInterface object_interface = {
    .type_name = "UtSharedMemorySubarray",
    .to_string = ut_shared_memory_subarray_to_string,
    .cleanup = ut_shared_memory_subarray_cleanup,
    .interfaces = {{&ut_uint8_list_id, &uint8_list_interface},
                   {&ut_list_id, &list_interface},
                   {NULL, NULL}}};

UtObject *ut_shared_memory_subarray_new(UtObject *parent, size_t start,
                                        size_t length) {
  UtObject *object =
      ut_object_new(sizeof(UtSharedMemorySubarray), &object_interface);
  UtSharedMemorySubarray *self = (UtSharedMemorySubarray *)object;

  assert(parent != NULL && ut_object_is_shared_memory_array(parent));
  size_t parent_length = ut_list_get_length(parent);
  assert(start + length <= parent_length);

  self->parent = ut_object_ref(parent);
  self->parent_length = parent_length;
  self->start = start;
  self->length = length;
  return object;
}

uint8_t *ut_shared_memory_subarray_get_data(UtObject *object) {
  assert(ut_object_is_shared_memory_subarray(object));
  UtSharedMemorySubarray *self = (UtSharedMemorySubarray *)object;
  return get_data(self);
}

bool ut_object_is_shared_memory_subarray(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
