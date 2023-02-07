#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  char *description;
} UtGzipError;

static char *ut_gzip_error_to_string(UtObject *object) {
  UtGzipError *self = (UtGzipError *)object;
  return ut_cstring_new_printf("<UtGzipError>(\"%s\")", self->description);
}

static void ut_gzip_error_cleanup(UtObject *object) {
  UtGzipError *self = (UtGzipError *)object;
  free(self->description);
}

static char *ut_gzip_error_get_description(UtObject *object) {
  UtGzipError *self = (UtGzipError *)object;
  return ut_cstring_new(self->description);
}

static UtErrorInterface error_interface = {.get_description =
                                               ut_gzip_error_get_description};

static UtObjectInterface object_interface = {
    .type_name = "UtGzipError",
    .to_string = ut_gzip_error_to_string,
    .cleanup = ut_gzip_error_cleanup,
    .interfaces = {{&ut_error_id, &error_interface}, {NULL, NULL}}};

UtObject *ut_gzip_error_new(const char *description) {
  UtObject *object = ut_object_new(sizeof(UtGzipError), &object_interface);
  UtGzipError *self = (UtGzipError *)object;
  self->description = ut_cstring_new(description);
  return object;
}

bool ut_object_is_gzip_error(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
