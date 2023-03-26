#include <assert.h>

#include "ut.h"

int ut_int64_list_id = 0;

UtObject *ut_int64_list_new() { return ut_int64_array_new(); }

UtObject *ut_int64_list_new_from_elements(size_t length, ...) {
  va_list ap;
  va_start(ap, length);
  return ut_int64_array_new_from_va_elements(length, ap);
  va_end(ap);
}

int64_t ut_int64_list_get_element(UtObject *object, size_t index) {
  UtInt64ListInterface *int64_list_interface =
      ut_object_get_interface(object, &ut_int64_list_id);
  assert(int64_list_interface != NULL);
  return int64_list_interface->get_element(object, index);
}

const int64_t *ut_int64_list_get_data(UtObject *object) {
  UtInt64ListInterface *int64_list_interface =
      ut_object_get_interface(object, &ut_int64_list_id);
  assert(int64_list_interface != NULL);
  if (int64_list_interface->get_data != NULL) {
    return int64_list_interface->get_data(object);
  } else {
    return NULL;
  }
}

int64_t *ut_int64_list_get_writable_data(UtObject *object) {
  UtInt64ListInterface *int64_list_interface =
      ut_object_get_interface(object, &ut_int64_list_id);
  assert(int64_list_interface != NULL);
  if (int64_list_interface->get_writable_data != NULL) {
    return int64_list_interface->get_writable_data(object);
  } else {
    return NULL;
  }
}

int64_t *ut_int64_list_take_data(UtObject *object) {
  UtInt64ListInterface *int64_list_interface =
      ut_object_get_interface(object, &ut_int64_list_id);
  assert(int64_list_interface != NULL);
  return int64_list_interface->take_data(object);
}

void ut_int64_list_append(UtObject *object, int64_t item) {
  ut_int64_list_append_block(object, &item, 1);
}

void ut_int64_list_append_block(UtObject *object, const int64_t *data,
                                size_t data_length) {
  size_t length = ut_list_get_length(object);
  ut_int64_list_insert_block(object, length, data, data_length);
}

void ut_int64_list_prepend(UtObject *object, int64_t item) {
  ut_int64_list_prepend_block(object, &item, 1);
}

void ut_int64_list_prepend_block(UtObject *object, const int64_t *data,
                                 size_t data_length) {
  ut_int64_list_insert_block(object, 0, data, data_length);
}

void ut_int64_list_insert(UtObject *object, size_t index, int64_t item) {
  ut_int64_list_insert_block(object, index, &item, 1);
}

void ut_int64_list_insert_block(UtObject *object, size_t index,
                                const int64_t *data, size_t data_length) {
  UtInt64ListInterface *int64_list_interface =
      ut_object_get_interface(object, &ut_int64_list_id);
  assert(int64_list_interface != NULL);
  assert(ut_list_is_mutable(object));
  int64_list_interface->insert(object, index, data, data_length);
}

bool ut_object_implements_int64_list(UtObject *object) {
  return ut_object_get_interface(object, &ut_int64_list_id) != NULL;
}
