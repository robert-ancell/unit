#include "ut.h"

typedef struct {
  UtObject object;
  char *description;
} UtExifError;

static char *ut_exif_error_to_string(UtObject *object) {
  UtExifError *self = (UtExifError *)object;
  return ut_cstring_new_printf("<UtExifError>(\"%s\")", self->description);
}

static void ut_exif_error_cleanup(UtObject *object) {
  UtExifError *self = (UtExifError *)object;
  free(self->description);
}

static char *ut_exif_error_get_description(UtObject *object) {
  UtExifError *self = (UtExifError *)object;
  return ut_cstring_new(self->description);
}

static UtErrorInterface error_interface = {.get_description =
                                               ut_exif_error_get_description};

static UtObjectInterface object_interface = {
    .type_name = "UtExifError",
    .to_string = ut_exif_error_to_string,
    .cleanup = ut_exif_error_cleanup,
    .interfaces = {{&ut_error_id, &error_interface}, {NULL, NULL}}};

UtObject *ut_exif_error_new(const char *description) {
  UtObject *object = ut_object_new(sizeof(UtExifError), &object_interface);
  UtExifError *self = (UtExifError *)object;
  self->description = ut_cstring_new(description);
  return object;
}

bool ut_object_is_exif_error(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
