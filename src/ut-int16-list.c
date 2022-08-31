#include <assert.h>

#include "ut.h"

int ut_int16_list_id = 0;

UtObject *ut_int16_list_new() { return ut_int16_array_new(); }

UtObject *ut_int16_list_new_from_elements(size_t length, ...) {
  va_list ap;
  va_start(ap, length);
  return ut_int16_array_new_from_va_elements(length, ap);
  va_end(ap);
}

int16_t ut_int16_list_get_element(UtObject *object, size_t index) {
  UtInt16ListInterface *int16_list_interface =
      ut_object_get_interface(object, &ut_int16_list_id);
  assert(int16_list_interface != NULL);
  return int16_list_interface->get_element(object, index);
}

const int16_t *ut_int16_list_get_data(UtObject *object) {
  UtInt16ListInterface *int16_list_interface =
      ut_object_get_interface(object, &ut_int16_list_id);
  assert(int16_list_interface != NULL);
  return int16_list_interface->get_data(object);
}

int16_t *ut_int16_list_take_data(UtObject *object) {
  UtInt16ListInterface *int16_list_interface =
      ut_object_get_interface(object, &ut_int16_list_id);
  assert(int16_list_interface != NULL);
  return int16_list_interface->take_data(object);
}

void ut_int16_list_append(UtObject *object, int16_t item) {
  ut_int16_list_append_block(object, &item, 1);
}

void ut_int16_list_append_block(UtObject *object, const int16_t *data,
                                size_t data_length) {
  size_t length = ut_list_get_length(object);
  ut_int16_list_insert_block(object, length, data, data_length);
}

void ut_int16_list_prepend(UtObject *object, int16_t item) {
  ut_int16_list_prepend_block(object, &item, 1);
}

void ut_int16_list_prepend_block(UtObject *object, const int16_t *data,
                                 size_t data_length) {
  ut_int16_list_insert_block(object, 0, data, data_length);
}

void ut_int16_list_insert(UtObject *object, size_t index, int16_t item) {
  ut_int16_list_insert_block(object, index, &item, 1);
}

void ut_int16_list_insert_block(UtObject *object, size_t index,
                                const int16_t *data, size_t data_length) {
  UtInt16ListInterface *int16_list_interface =
      ut_object_get_interface(object, &ut_int16_list_id);
  assert(int16_list_interface != NULL);
  assert(ut_list_is_mutable(object));
  int16_list_interface->insert(object, index, data, data_length);
}

bool ut_object_implements_int16_list(UtObject *object) {
  return ut_object_get_interface(object, &ut_int16_list_id) != NULL;
}
