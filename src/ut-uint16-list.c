#include <assert.h>

#include "ut.h"

int ut_uint16_list_id = 0;

UtObject *ut_uint16_list_new() { return ut_uint16_array_new(); }

UtObject *ut_uint16_list_new_from_elements(size_t length, ...) {
  va_list ap;
  va_start(ap, length);
  return ut_uint16_array_new_from_va_elements(length, ap);
  va_end(ap);
}

UtObject *ut_uint16_list_new_from_hex_string(const char *hex) {
  return ut_uint16_array_new_from_hex_string(hex);
}

uint16_t ut_uint16_list_get_element(UtObject *object, size_t index) {
  UtUint16ListInterface *uint16_list_interface =
      ut_object_get_interface(object, &ut_uint16_list_id);
  assert(uint16_list_interface != NULL);
  return uint16_list_interface->get_element(object, index);
}

const uint16_t *ut_uint16_list_get_data(UtObject *object) {
  UtUint16ListInterface *uint16_list_interface =
      ut_object_get_interface(object, &ut_uint16_list_id);
  assert(uint16_list_interface != NULL);
  return uint16_list_interface->get_data(object);
}

uint16_t *ut_uint16_list_get_writable_data(UtObject *object) {
  UtUint16ListInterface *uint16_list_interface =
      ut_object_get_interface(object, &ut_uint16_list_id);
  assert(uint16_list_interface != NULL);
  if (uint16_list_interface->get_writable_data != NULL) {
    return uint16_list_interface->get_writable_data(object);
  } else {
    return NULL;
  }
}

uint16_t *ut_uint16_list_take_data(UtObject *object) {
  UtUint16ListInterface *uint16_list_interface =
      ut_object_get_interface(object, &ut_uint16_list_id);
  assert(uint16_list_interface != NULL);
  return uint16_list_interface->take_data(object);
}

void ut_uint16_list_append(UtObject *object, uint16_t value) {
  ut_uint16_list_append_block(object, &value, 1);
}

void ut_uint16_list_append_block(UtObject *object, const uint16_t *data,
                                 size_t data_length) {
  size_t length = ut_list_get_length(object);
  ut_uint16_list_insert_block(object, length, data, data_length);
}

void ut_uint16_list_prepend(UtObject *object, uint16_t value) {
  ut_uint16_list_prepend_block(object, &value, 1);
}

void ut_uint16_list_prepend_block(UtObject *object, const uint16_t *data,
                                  size_t data_length) {
  ut_uint16_list_insert_block(object, 0, data, data_length);
}

void ut_uint16_list_insert(UtObject *object, size_t index, uint16_t value) {
  ut_uint16_list_insert_block(object, index, &value, 1);
}

void ut_uint16_list_insert_block(UtObject *object, size_t index,
                                 const uint16_t *data, size_t data_length) {
  UtUint16ListInterface *uint16_list_interface =
      ut_object_get_interface(object, &ut_uint16_list_id);
  assert(uint16_list_interface != NULL);
  assert(ut_list_is_mutable(object));
  uint16_list_interface->insert(object, index, data, data_length);
}

char *ut_uint16_list_to_hex_string(UtObject *object) {
  UtUint16ListInterface *uint16_list_interface =
      ut_object_get_interface(object, &ut_uint16_list_id);
  assert(uint16_list_interface != NULL);

  size_t length = ut_list_get_length(object);
  const uint16_t *data = ut_uint16_list_get_data(object);
  UtObjectRef hex_string = ut_string_new("");
  if (data != NULL) {
    for (size_t i = 0; i < length; i++) {
      ut_string_append_printf(hex_string, "%04x", data[i]);
    }
  } else {
    for (size_t i = 0; i < length; i++) {
      ut_string_append_printf(hex_string, "%04x",
                              ut_uint16_list_get_element(object, i));
    }
  }

  return ut_string_take_text(hex_string);
}

bool ut_object_implements_uint16_list(UtObject *object) {
  return ut_object_get_interface(object, &ut_uint16_list_id) != NULL;
}
