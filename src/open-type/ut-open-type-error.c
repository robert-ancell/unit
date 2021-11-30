#include "ut.h"

typedef struct {
  UtObject object;
  char *description;
} UtOpenTypeError;

static char *ut_open_type_error_to_string(UtObject *object) {
  UtOpenTypeError *self = (UtOpenTypeError *)object;
  return ut_cstring_new_printf("<UtOpenTypeError>(\"%s\")", self->description);
}

static void ut_open_type_error_cleanup(UtObject *object) {
  UtOpenTypeError *self = (UtOpenTypeError *)object;
  free(self->description);
}

static char *ut_open_type_error_get_description(UtObject *object) {
  UtOpenTypeError *self = (UtOpenTypeError *)object;
  return ut_cstring_new(self->description);
}

static UtErrorInterface error_interface = {
    .get_description = ut_open_type_error_get_description};

static UtObjectInterface object_interface = {
    .type_name = "UtOpenTypeError",
    .to_string = ut_open_type_error_to_string,
    .cleanup = ut_open_type_error_cleanup,
    .interfaces = {{&ut_error_id, &error_interface}, {NULL, NULL}}};

UtObject *ut_open_type_error_new(const char *description) {
  UtObject *object = ut_object_new(sizeof(UtOpenTypeError), &object_interface);
  UtOpenTypeError *self = (UtOpenTypeError *)object;
  self->description = ut_cstring_new(description);
  return object;
}

bool ut_object_is_open_type_error(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
