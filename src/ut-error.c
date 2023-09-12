#include <assert.h>

#include "ut.h"

int ut_error_id = 0;

UtObject *ut_error_new(const char *description) {
  return ut_general_error_new(description);
}

UtObject *ut_error_new_take(char *description) {
  return ut_general_error_new_take(description);
}

char *ut_error_get_description(UtObject *object) {
  UtErrorInterface *error_interface =
      ut_object_get_interface(object, &ut_error_id);
  assert(error_interface != NULL);
  return error_interface->get_description(object);
}

bool ut_object_implements_error(UtObject *object) {
  return ut_object_get_interface(object, &ut_error_id) != NULL;
}
