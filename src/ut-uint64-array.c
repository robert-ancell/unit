#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>

#include "ut-end-of-stream.h"
#include "ut-input-stream.h"
#include "ut-list.h"
#include "ut-object-private.h"
#include "ut-uint64-array.h"
#include "ut-uint64-list.h"
#include "ut-uint64.h"

typedef struct {
  UtObject object;
  uint64_t *data;
  size_t data_length;
} UtUint64Array;

static void resize_list(UtUint64Array *self, size_t length) {
  self->data = realloc(self->data, sizeof(uint64_t) * length);
  for (size_t i = self->data_length; i < length; i++) {
    self->data[i] = 0;
  }
  self->data_length = length;
}

static uint64_t ut_uint64_array_get_element(UtObject *object, size_t index) {
  UtUint64Array *self = (UtUint64Array *)object;
  return self->data[index];
}

static void ut_uint64_array_insert_object(UtObject *object, size_t index,
                                          UtObject *item) {
  assert(ut_object_is_uint64(item));
  ut_uint64_array_insert(object, index, ut_uint64_get_value(item));
}

static void ut_uint64_array_remove(UtObject *object, size_t index,
                                   size_t count) {
  UtUint64Array *self = (UtUint64Array *)object;
  assert(index <= self->data_length);
  assert(index + count <= self->data_length);
  for (size_t i = index; i < self->data_length - count; i++) {
    self->data[i] = self->data[i + count];
  }
  resize_list(self, self->data_length - count);
}

static void ut_uint64_array_resize(UtObject *object, size_t length) {
  UtUint64Array *self = (UtUint64Array *)object;
  resize_list(self, length);
}

static size_t ut_uint64_array_get_length(UtObject *object) {
  UtUint64Array *self = (UtUint64Array *)object;
  return self->data_length;
}

static UtObject *ut_uint64_array_get_element_object(UtObject *object,
                                                    size_t index) {
  UtUint64Array *self = (UtUint64Array *)object;
  return ut_uint64_new(self->data[index]);
}

static UtObject *ut_uint64_array_copy(UtObject *object) {
  UtUint64Array *self = (UtUint64Array *)object;
  UtObject *copy = ut_uint64_array_new();
  ut_uint64_array_append_block(copy, self->data, self->data_length);
  return copy;
}

static void ut_uint64_array_read(UtObject *object,
                                 UtInputStreamCallback callback,
                                 void *user_data, UtObject *cancel) {
  UtUint64Array *self = (UtUint64Array *)object;
  size_t n_used = callback(user_data, object);
  UtObjectRef unused_data = NULL;
  if (n_used != self->data_length) {
    unused_data = ut_uint64_array_new();
    ut_uint64_array_append_block(unused_data, self->data + n_used,
                                 self->data_length - n_used);
  }
  UtObjectRef eos = ut_end_of_stream_new(unused_data);
  callback(user_data, eos);
}

static void ut_uint64_array_init(UtObject *object) {
  UtUint64Array *self = (UtUint64Array *)object;
  self->data = NULL;
  self->data_length = 0;
}

static void ut_uint64_array_cleanup(UtObject *object) {
  UtUint64Array *self = (UtUint64Array *)object;
  free(self->data);
}

static UtUint64ListInterface uint64_list_interface = {
    .get_element = ut_uint64_array_get_element};

static UtListInterface list_interface = {
    .is_mutable = true,
    .get_length = ut_uint64_array_get_length,
    .get_element = ut_uint64_array_get_element_object,
    .copy = ut_uint64_array_copy,
    .insert = ut_uint64_array_insert_object,
    .remove = ut_uint64_array_remove,
    .resize = ut_uint64_array_resize};

static UtInputStreamInterface input_stream_interface = {
    .read = ut_uint64_array_read, .read_all = ut_uint64_array_read};

static UtObjectInterface object_interface = {
    .type_name = "UtUint64Array",
    .init = ut_uint64_array_init,
    .to_string = ut_list_to_string,
    .cleanup = ut_uint64_array_cleanup,
    .interfaces = {{&ut_uint64_list_id, &uint64_list_interface},
                   {&ut_list_id, &list_interface},
                   {&ut_input_stream_id, &input_stream_interface},
                   {NULL, NULL}}};

UtObject *ut_uint64_array_new() {
  return ut_object_new(sizeof(UtUint64Array), &object_interface);
}

UtObject *ut_uint64_array_new_with_data(size_t length, ...) {
  UtObject *object = ut_uint64_array_new();
  UtUint64Array *self = (UtUint64Array *)object;

  resize_list(self, length);
  va_list ap;
  va_start(ap, length);
  for (size_t i = 0; i < length; i++) {
    self->data[i] = va_arg(ap, int);
  }
  va_end(ap);

  return object;
}

void ut_uint64_array_append(UtObject *object, uint64_t data) {
  ut_uint64_array_append_block(object, &data, 1);
}

void ut_uint64_array_append_block(UtObject *object, const uint64_t *data,
                                  size_t data_length) {
  assert(ut_object_is_uint64_array(object));
  UtUint64Array *self = (UtUint64Array *)object;
  ut_uint64_array_insert_block(object, self->data_length, data, data_length);
}

void ut_uint64_array_insert(UtObject *object, size_t index, uint64_t data) {
  ut_uint64_array_insert_block(object, index, &data, 1);
}

void ut_uint64_array_insert_block(UtObject *object, size_t index,
                                  const uint64_t *data, size_t data_length) {
  assert(ut_object_is_uint64_array(object));
  UtUint64Array *self = (UtUint64Array *)object;

  size_t orig_data_length = self->data_length;
  resize_list(self, self->data_length + data_length);

  // Shift existing data up
  for (size_t i = index; i < orig_data_length; i++) {
    size_t new_index = self->data_length - i - 1;
    size_t old_index = new_index - data_length;
    self->data[new_index] = self->data[old_index];
  }

  // Insert new data
  for (size_t i = 0; i < data_length; i++) {
    self->data[index + i] = data[i];
  }
}

uint64_t *ut_uint64_array_get_data(UtObject *object) {
  assert(ut_object_is_uint64_array(object));
  UtUint64Array *self = (UtUint64Array *)object;
  return self->data;
}

bool ut_object_is_uint64_array(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}