#include "ut.h"

typedef struct {
  UtObject object;
  char *description;
} UtTiffError;

static char *ut_tiff_error_to_string(UtObject *object) {
  UtTiffError *self = (UtTiffError *)object;
  return ut_cstring_new_printf("<UtTiffError>(\"%s\")", self->description);
}

static void ut_tiff_error_cleanup(UtObject *object) {
  UtTiffError *self = (UtTiffError *)object;
  free(self->description);
}

static char *ut_tiff_error_get_description(UtObject *object) {
  UtTiffError *self = (UtTiffError *)object;
  return ut_cstring_new(self->description);
}

static UtErrorInterface error_interface = {.get_description =
                                               ut_tiff_error_get_description};

static UtObjectInterface object_interface = {
    .type_name = "UtTiffError",
    .to_string = ut_tiff_error_to_string,
    .cleanup = ut_tiff_error_cleanup,
    .interfaces = {{&ut_error_id, &error_interface}, {NULL, NULL}}};

UtObject *ut_tiff_error_new(const char *description) {
  UtObject *object = ut_object_new(sizeof(UtTiffError), &object_interface);
  UtTiffError *self = (UtTiffError *)object;
  self->description = ut_cstring_new(description);
  return object;
}

bool ut_object_is_tiff_error(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
