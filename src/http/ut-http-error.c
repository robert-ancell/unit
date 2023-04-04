#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  char *description;
} UtHttpError;

static char *ut_http_error_to_string(UtObject *object) {
  UtHttpError *self = (UtHttpError *)object;
  return ut_cstring_new_printf("<UtHttpError>(\"%s\")", self->description);
}

static void ut_http_error_cleanup(UtObject *object) {
  UtHttpError *self = (UtHttpError *)object;
  free(self->description);
}

static char *ut_http_error_get_description(UtObject *object) {
  UtHttpError *self = (UtHttpError *)object;
  return ut_cstring_new(self->description);
}

static UtErrorInterface error_interface = {.get_description =
                                               ut_http_error_get_description};

static UtObjectInterface object_interface = {
    .type_name = "UtHttpError",
    .to_string = ut_http_error_to_string,
    .cleanup = ut_http_error_cleanup,
    .interfaces = {{&ut_error_id, &error_interface}, {NULL, NULL}}};

UtObject *ut_http_error_new(const char *description) {
  UtObject *object = ut_object_new(sizeof(UtHttpError), &object_interface);
  UtHttpError *self = (UtHttpError *)object;
  self->description = ut_cstring_new(description);
  return object;
}

bool ut_object_is_http_error(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
