#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ut.h"

typedef struct {
  UtObject object;
  uint8_t *data;
  size_t bit_count;
} UtBitArray;

static size_t byte_count(size_t bit_count) { return (bit_count + 7) / 8; }

static void resize_list(UtBitArray *self, size_t bit_count) {
  size_t orig_data_length = byte_count(self->bit_count);
  size_t data_length = byte_count(bit_count);
  self->data = realloc(self->data, sizeof(uint8_t) * data_length);
  for (size_t i = orig_data_length; i < data_length; i++) {
    self->data[i] = 0;
  }
  self->bit_count = bit_count;
}

static void set_element(UtBitArray *self, size_t index, bool value) {
  size_t byte_index = index / 8;
  size_t bit_index = index % 8;
  uint8_t mask = 1 << (7 - bit_index);
  if (value) {
    self->data[byte_index] |= mask;
  } else {
    self->data[byte_index] &= 0xff ^ mask;
  }
}

static bool get_element(UtBitArray *self, size_t index) {
  size_t byte_index = index / 8;
  size_t bit_index = index % 8;
  uint8_t mask = 1 << (7 - bit_index);
  return (self->data[byte_index] & mask) != 0;
}

static void append(UtBitArray *self, uint8_t data, size_t bit_count) {
  size_t byte_index = self->bit_count / 8;
  size_t bit_index = self->bit_count % 8;
  resize_list(self, self->bit_count + bit_count);

  // Shift bits into MSB.
  data <<= 8 - bit_count;

  self->data[byte_index] |= data >> bit_index;
  if (bit_index + bit_count > 8) {
    self->data[byte_index + 1] = (data << (8 - bit_index));
  }
}

static bool ut_bit_array_get_element(UtObject *object, size_t index) {
  UtBitArray *self = (UtBitArray *)object;
  return get_element(self, index);
}

static const uint8_t *ut_bit_array_get_const_data(UtObject *object) {
  UtBitArray *self = (UtBitArray *)object;
  return self->data;
}

static uint8_t *ut_bit_array_take_data(UtObject *object) {
  UtBitArray *self = (UtBitArray *)object;
  uint8_t *result = self->data;
  self->data = NULL;
  self->bit_count = 0;
  return result;
}

static void ut_bit_array_insert(UtObject *object, size_t index, uint8_t data,
                                size_t bit_count) {
  UtBitArray *self = (UtBitArray *)object;

  if (index == self->bit_count) {
    append(self, data, bit_count);
    return;
  }

  resize_list(self, self->bit_count + bit_count);

  // Shift bits right to make space to insert.
  // FIXME: Very inefficient, could copy blocks of bits.
  for (size_t i = self->bit_count; i >= index + bit_count; i--) {
    set_element(self, i, get_element(self, i - bit_count));
  }

  // Insert new bits.
  // FIXME: Very inefficient, could copy blocks of bits.
  for (size_t i = 0; i < bit_count; i++) {
    uint8_t mask = 1 << (bit_count - i - 1);
    set_element(self, index + i, (data & mask) != 0);
  }
}

static void ut_bit_array_append(UtObject *object, uint8_t data,
                                size_t bit_count) {
  UtBitArray *self = (UtBitArray *)object;
  append(self, data, bit_count);
}

static void ut_bit_array_insert_object(UtObject *object, size_t index,
                                       UtObject *item) {
  assert(ut_object_is_boolean(item));
  bool value = ut_boolean_get_value(item);
  ut_bit_array_insert(object, index, value ? 0x01 : 0x00, 1);
}

static void ut_bit_array_remove(UtObject *object, size_t index, size_t count) {
  UtBitArray *self = (UtBitArray *)object;

  // Shift following bits left.
  // FIXME: Very inefficient, could copy blocks of bits.
  for (size_t i = index + count; i < self->bit_count; i++) {
    set_element(self, i - count, get_element(self, i));
  }

  resize_list(self, self->bit_count - count);
}

static void ut_bit_array_resize(UtObject *object, size_t length) {
  UtBitArray *self = (UtBitArray *)object;
  resize_list(self, length);
}

static size_t ut_bit_array_get_length(UtObject *object) {
  UtBitArray *self = (UtBitArray *)object;
  return self->bit_count;
}

static UtObject *ut_bit_array_get_element_object(UtObject *object,
                                                 size_t index) {
  UtBitArray *self = (UtBitArray *)object;
  return ut_boolean_new(get_element(self, index));
}

static UtObject *ut_bit_array_copy(UtObject *object) {
  UtBitArray *self = (UtBitArray *)object;

  UtObject *copy = ut_bit_array_new();
  UtBitArray *copy_self = (UtBitArray *)copy;

  size_t data_length = byte_count(self->bit_count);
  copy_self->data = realloc(copy_self->data, data_length);
  copy_self->bit_count = self->bit_count;
  for (size_t i = 0; i < data_length; i++) {
    copy_self->data[i] = self->data[i];
  }

  return copy;
}

static char *ut_bit_array_to_string(UtObject *object) {
  UtBitArray *self = (UtBitArray *)object;
  UtObjectRef string = ut_string_new("<bool>[");
  for (size_t i = 0; i < self->bit_count; i++) {
    if (i != 0) {
      ut_string_append(string, ", ");
    }
    ut_string_append(string, get_element(self, i) ? "true" : "false");
  }
  ut_string_append(string, "]");

  return ut_string_take_text(string);
}

static void ut_bit_array_cleanup(UtObject *object) {
  UtBitArray *self = (UtBitArray *)object;
  free(self->data);
}

static UtBitListInterface bit_list_interface = {
    .get_element = ut_bit_array_get_element,
    .get_data = ut_bit_array_get_const_data,
    .take_data = ut_bit_array_take_data,
    .insert = ut_bit_array_insert,
    .append = ut_bit_array_append};

static UtListInterface list_interface = {.is_mutable = true,
                                         .get_length = ut_bit_array_get_length,
                                         .get_element =
                                             ut_bit_array_get_element_object,
                                         .copy = ut_bit_array_copy,
                                         .insert = ut_bit_array_insert_object,
                                         .remove = ut_bit_array_remove,
                                         .resize = ut_bit_array_resize};

static UtObjectInterface object_interface = {
    .type_name = "UtBitArray",
    .to_string = ut_bit_array_to_string,
    .cleanup = ut_bit_array_cleanup,
    .interfaces = {{&ut_bit_list_id, &bit_list_interface},
                   {&ut_list_id, &list_interface},
                   {NULL, NULL}}};

UtObject *ut_bit_array_new() {
  return ut_object_new(sizeof(UtBitArray), &object_interface);
}

UtObject *ut_bit_array_new_from_data(const uint8_t *data, size_t bit_count) {
  UtObject *object = ut_object_new(sizeof(UtBitArray), &object_interface);
  UtBitArray *self = (UtBitArray *)object;

  size_t data_length = byte_count(bit_count);
  self->data = malloc(sizeof(uint8_t) * data_length);
  memcpy(self->data, data, data_length);
  self->bit_count = bit_count;

  // FIXME: Clear unused bits

  return object;
}

UtObject *ut_bit_array_new_from_bin_string(const char *bin) {
  size_t length = strlen(bin);

  UtObjectRef object = ut_bit_array_new();
  UtBitArray *self = (UtBitArray *)object;
  resize_list(self, length);
  for (size_t i = 0; i < length; i++) {
    switch (bin[i]) {
    case '0':
      break;
    case '1':
      self->data[i / 8] |= 1 << (7 - i % 8);
      break;
    default:
      return ut_error_new("Invalid binary string");
    }
  }

  return ut_object_ref(object);
}

uint8_t *ut_bit_array_get_data(UtObject *object) {
  assert(ut_object_is_bit_array(object));
  UtBitArray *self = (UtBitArray *)object;
  return self->data;
}

bool ut_object_is_bit_array(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
