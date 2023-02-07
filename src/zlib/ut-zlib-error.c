#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  char *description;
} UtZlibError;

static void ut_zlib_error_cleanup(UtObject *object) {
  UtZlibError *self = (UtZlibError *)object;
  free(self->description);
}

static char *ut_zlib_error_get_description(UtObject *object) {
  UtZlibError *self = (UtZlibError *)object;
  return ut_cstring_new(self->description);
}

static UtErrorInterface error_interface = {.get_description =
                                               ut_zlib_error_get_description};

static UtObjectInterface object_interface = {
    .type_name = "UtZlibError",
    .cleanup = ut_zlib_error_cleanup,
    .interfaces = {{&ut_error_id, &error_interface}, {NULL, NULL}}};

UtObject *ut_zlib_error_new(const char *description) {
  UtObject *object = ut_object_new(sizeof(UtZlibError), &object_interface);
  UtZlibError *self = (UtZlibError *)object;
  self->description = ut_cstring_new(description);
  return object;
}

bool ut_object_is_zlib_error(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
