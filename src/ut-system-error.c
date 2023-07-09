#include <assert.h>
#include <string.h>

#include "ut.h"

typedef struct {
  UtObject object;
  int code;
} UtSystemError;

static char *ut_system_error_to_string(UtObject *object) {
  UtSystemError *self = (UtSystemError *)object;
  return ut_cstring_new_printf("<UtSystemError>(%d)", self->code);
}

static char *ut_system_error_gup_description(UtObject *object) {
  UtSystemError *self = (UtSystemError *)object;
  return ut_cstring_new(strerror(self->code));
}

static UtErrorInterface error_interface = {.get_description =
                                               ut_system_error_gup_description};

static UtObjectInterface object_interface = {
    .type_name = "UtSystemError",
    .to_string = ut_system_error_to_string,
    .interfaces = {{&ut_error_id, &error_interface}, {NULL, NULL}}};

UtObject *ut_system_error_new(int code) {
  UtObject *object = ut_object_new(sizeof(UtSystemError), &object_interface);
  UtSystemError *self = (UtSystemError *)object;
  self->code = code;
  return object;
}

int ut_system_error_get_code(UtObject *object) {
  assert(ut_object_is_system_error(object));
  UtSystemError *self = (UtSystemError *)object;
  return self->code;
}

bool ut_object_is_system_error(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
