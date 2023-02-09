#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  char *description;
} UtDeflateError;

static char *ut_deflate_error_to_string(UtObject *object) {
  UtDeflateError *self = (UtDeflateError *)object;
  return ut_cstring_new_printf("<UtDeflateError>(\"%s\")", self->description);
}

static void ut_deflate_error_cleanup(UtObject *object) {
  UtDeflateError *self = (UtDeflateError *)object;
  free(self->description);
}

static char *ut_deflate_error_get_description(UtObject *object) {
  UtDeflateError *self = (UtDeflateError *)object;
  return ut_cstring_new(self->description);
}

static UtErrorInterface error_interface = {
    .get_description = ut_deflate_error_get_description};

static UtObjectInterface object_interface = {
    .type_name = "UtDeflateError",
    .to_string = ut_deflate_error_to_string,
    .cleanup = ut_deflate_error_cleanup,
    .interfaces = {{&ut_error_id, &error_interface}, {NULL, NULL}}};

UtObject *ut_deflate_error_new(const char *description) {
  UtObject *object = ut_object_new(sizeof(UtDeflateError), &object_interface);
  UtDeflateError *self = (UtDeflateError *)object;
  self->description = ut_cstring_new(description);
  return object;
}

bool ut_object_is_deflate_error(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
