#include <assert.h>

#include "ut.h"

int ut_float32_list_id = 0;

UtObject *ut_float32_list_new() { return ut_float32_array_new(); }

UtObject *ut_float32_list_new_from_elements(size_t length, ...) {
  va_list ap;
  va_start(ap, length);
  return ut_float32_array_new_from_va_elements(length, ap);
  va_end(ap);
}

float ut_float32_list_get_element(UtObject *object, size_t index) {
  UtFloat32ListInterface *float32_list_interface =
      ut_object_get_interface(object, &ut_float32_list_id);
  assert(float32_list_interface != NULL);
  return float32_list_interface->get_element(object, index);
}

const float *ut_float32_list_get_data(UtObject *object) {
  UtFloat32ListInterface *float32_list_interface =
      ut_object_get_interface(object, &ut_float32_list_id);
  assert(float32_list_interface != NULL);
  if (float32_list_interface->get_data != NULL) {
    return float32_list_interface->get_data(object);
  } else {
    return NULL;
  }
}

float *ut_float32_list_get_writable_data(UtObject *object) {
  UtFloat32ListInterface *float32_list_interface =
      ut_object_get_interface(object, &ut_float32_list_id);
  assert(float32_list_interface != NULL);
  if (float32_list_interface->get_writable_data != NULL) {
    return float32_list_interface->get_writable_data(object);
  } else {
    return NULL;
  }
}

float *ut_float32_list_take_data(UtObject *object) {
  UtFloat32ListInterface *float32_list_interface =
      ut_object_get_interface(object, &ut_float32_list_id);
  assert(float32_list_interface != NULL);
  return float32_list_interface->take_data(object);
}

void ut_float32_list_append(UtObject *object, float item) {
  ut_float32_list_append_block(object, &item, 1);
}

void ut_float32_list_append_block(UtObject *object, const float *data,
                                  size_t data_length) {
  size_t length = ut_list_get_length(object);
  ut_float32_list_insert_block(object, length, data, data_length);
}

void ut_float32_list_prepend(UtObject *object, float item) {
  ut_float32_list_prepend_block(object, &item, 1);
}

void ut_float32_list_prepend_block(UtObject *object, const float *data,
                                   size_t data_length) {
  ut_float32_list_insert_block(object, 0, data, data_length);
}

void ut_float32_list_insert(UtObject *object, size_t index, float item) {
  ut_float32_list_insert_block(object, index, &item, 1);
}

void ut_float32_list_insert_block(UtObject *object, size_t index,
                                  const float *data, size_t data_length) {
  UtFloat32ListInterface *float32_list_interface =
      ut_object_get_interface(object, &ut_float32_list_id);
  assert(float32_list_interface != NULL);
  assert(ut_list_is_mutable(object));
  float32_list_interface->insert(object, index, data, data_length);
}

bool ut_object_implements_float32_list(UtObject *object) {
  return ut_object_get_interface(object, &ut_float32_list_id) != NULL;
}
