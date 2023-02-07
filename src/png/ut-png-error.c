#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  char *description;
} UtPngError;

static char *ut_png_error_to_string(UtObject *object) {
  UtPngError *self = (UtPngError *)object;
  return ut_cstring_new_printf("<UtPngError>(\"%s\")", self->description);
}

static void ut_png_error_cleanup(UtObject *object) {
  UtPngError *self = (UtPngError *)object;
  free(self->description);
}

static char *ut_png_error_get_description(UtObject *object) {
  UtPngError *self = (UtPngError *)object;
  return ut_cstring_new(self->description);
}

static UtErrorInterface error_interface = {.get_description =
                                               ut_png_error_get_description};

static UtObjectInterface object_interface = {
    .type_name = "UtPngError",
    .to_string = ut_png_error_to_string,
    .cleanup = ut_png_error_cleanup,
    .interfaces = {{&ut_error_id, &error_interface}, {NULL, NULL}}};

UtObject *ut_png_error_new(const char *description) {
  UtObject *object = ut_object_new(sizeof(UtPngError), &object_interface);
  UtPngError *self = (UtPngError *)object;
  self->description = ut_cstring_new(description);
  return object;
}

bool ut_object_is_png_error(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
