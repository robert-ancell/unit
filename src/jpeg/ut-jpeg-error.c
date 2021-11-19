#include "ut.h"

typedef struct {
  UtObject object;
  char *description;
} UtJpegError;

static char *ut_jpeg_error_to_string(UtObject *object) {
  UtJpegError *self = (UtJpegError *)object;
  return ut_cstring_new_printf("<UtJpegError>(\"%s\")", self->description);
}

static void ut_jpeg_error_cleanup(UtObject *object) {
  UtJpegError *self = (UtJpegError *)object;
  free(self->description);
}

static char *ut_jpeg_error_get_description(UtObject *object) {
  UtJpegError *self = (UtJpegError *)object;
  return ut_cstring_new(self->description);
}

static UtErrorInterface error_interface = {.get_description =
                                               ut_jpeg_error_get_description};

static UtObjectInterface object_interface = {
    .type_name = "UtJpegError",
    .to_string = ut_jpeg_error_to_string,
    .cleanup = ut_jpeg_error_cleanup,
    .interfaces = {{&ut_error_id, &error_interface}, {NULL, NULL}}};

UtObject *ut_jpeg_error_new(const char *description) {
  UtObject *object = ut_object_new(sizeof(UtJpegError), &object_interface);
  UtJpegError *self = (UtJpegError *)object;
  self->description = ut_cstring_new(description);
  return object;
}

bool ut_object_is_jpeg_error(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
