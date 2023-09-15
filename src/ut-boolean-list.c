#include <assert.h>

#include "ut.h"

int ut_boolean_list_id = 0;

UtObject *ut_boolean_list_new() { return ut_boolean_array_new(); }

bool ut_boolean_list_get_element(UtObject *object, size_t index) {
  UtBooleanListInterface *boolean_list_interface =
      ut_object_get_interface(object, &ut_boolean_list_id);
  assert(boolean_list_interface != NULL);
  return boolean_list_interface->get_element(object, index);
}

void ut_boolean_list_append(UtObject *object, bool value) {
  size_t length = ut_list_get_length(object);
  ut_boolean_list_insert(object, length, value);
}

void ut_boolean_list_prepend(UtObject *object, bool value) {
  ut_boolean_list_insert(object, 0, value);
}

void ut_boolean_list_insert(UtObject *object, size_t index, bool value) {
  UtBooleanListInterface *boolean_list_interface =
      ut_object_get_interface(object, &ut_boolean_list_id);
  assert(boolean_list_interface != NULL);
  assert(ut_list_is_mutable(object));
  boolean_list_interface->insert(object, index, value);
}

bool ut_object_implements_boolean_list(UtObject *object) {
  return ut_object_get_interface(object, &ut_boolean_list_id) != NULL;
}
