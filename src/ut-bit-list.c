#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>

#include "ut.h"

int ut_bit_list_id = 0;

UtObject *ut_bit_list_new() { return ut_bit_array_new(); }

UtObject *ut_bit_list_new_from_bin_string(const char *bin) {
  return ut_bit_array_new_from_bin_string(bin);
}

bool ut_bit_list_get_element(UtObject *object, size_t index) {
  UtBitListInterface *bit_list_interface =
      ut_object_get_interface(object, &ut_bit_list_id);
  assert(bit_list_interface != NULL);
  return bit_list_interface->get_element(object, index);
}

const uint8_t *ut_bit_list_get_data(UtObject *object) {
  UtBitListInterface *bit_list_interface =
      ut_object_get_interface(object, &ut_bit_list_id);
  assert(bit_list_interface != NULL);
  if (bit_list_interface->get_data != NULL) {
    return bit_list_interface->get_data(object);
  } else {
    return NULL;
  }
}

uint8_t *ut_bit_list_copy_data(UtObject *object) {
  UtBitListInterface *bit_list_interface =
      ut_object_get_interface(object, &ut_bit_list_id);
  assert(bit_list_interface != NULL);
  const uint8_t *data = ut_bit_list_get_data(object);
  size_t data_length = ut_list_get_length(object); // FIXME
  uint8_t *data_copy = malloc(data_length);
  if (data != NULL) {
    for (size_t i = 0; i < data_length; i++) {
      data_copy[i] = data[i];
    }
  } else {
    for (size_t i = 0; i < data_length; i++) {
      data_copy[i] = bit_list_interface->get_element(object, i);
    }
  }

  return data_copy;
}

uint8_t *ut_bit_list_take_data(UtObject *object) {
  UtBitListInterface *bit_list_interface =
      ut_object_get_interface(object, &ut_bit_list_id);
  assert(bit_list_interface != NULL);
  return bit_list_interface->take_data(object);
}

void ut_bit_list_append(UtObject *object, bool value) {
  ut_bit_list_append_bits(object, value ? 0x01 : 0x00, 1);
}

void ut_bit_list_append_bits(UtObject *object, uint8_t data, size_t bit_count) {
  UtBitListInterface *bit_list_interface =
      ut_object_get_interface(object, &ut_bit_list_id);
  assert(bit_list_interface != NULL);
  assert(ut_list_is_mutable(object));
  bit_list_interface->append(object, data, bit_count);
}

void ut_bit_list_prepend(UtObject *object, bool value) {
  ut_bit_list_prepend_bits(object, value ? 0x01 : 0x00, 1);
}

void ut_bit_list_prepend_bits(UtObject *object, uint8_t data,
                              size_t bit_count) {
  ut_bit_list_insert_bits(object, 0, data, bit_count);
}

void ut_bit_list_insert(UtObject *object, size_t index, bool value) {
  ut_bit_list_insert_bits(object, index, value ? 0x01 : 0x00, 1);
}

void ut_bit_list_insert_bits(UtObject *object, size_t index, uint8_t data,
                             size_t bit_count) {
  UtBitListInterface *bit_list_interface =
      ut_object_get_interface(object, &ut_bit_list_id);
  assert(bit_list_interface != NULL);
  assert(ut_list_is_mutable(object));
  bit_list_interface->insert(object, index, data, bit_count);
}

char *ut_bit_list_to_bin_string(UtObject *object) {
  UtBitListInterface *bit_list_interface =
      ut_object_get_interface(object, &ut_bit_list_id);
  assert(bit_list_interface != NULL);

  size_t length = ut_list_get_length(object);
  const uint8_t *data = ut_bit_list_get_data(object);
  UtObjectRef bin_string = ut_string_new("");
  if (data != NULL) {
    for (size_t i = 0; i < length; i++) {
      uint8_t mask = 1 << (7 - (i % 8));
      bool is_set = (data[i / 8] & mask) != 0;
      ut_string_append(bin_string, is_set ? "1" : "0");
    }
  } else {
    for (size_t i = 0; i < length; i++) {
      ut_string_append(bin_string,
                       ut_bit_list_get_element(object, i) ? "1" : "0");
    }
  }

  return ut_string_take_text(bin_string);
}

bool ut_object_implements_bit_list(UtObject *object) {
  return ut_object_get_interface(object, &ut_bit_list_id) != NULL;
}
