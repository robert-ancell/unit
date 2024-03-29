#include "ut.h"

typedef struct {
  UtObject object;
  char *description;
} UtGifError;

static char *ut_gif_error_to_string(UtObject *object) {
  UtGifError *self = (UtGifError *)object;
  return ut_cstring_new_printf("<UtGifError>(\"%s\")", self->description);
}

static void ut_gif_error_cleanup(UtObject *object) {
  UtGifError *self = (UtGifError *)object;
  free(self->description);
}

static char *ut_gif_error_get_description(UtObject *object) {
  UtGifError *self = (UtGifError *)object;
  return ut_cstring_new(self->description);
}

static UtErrorInterface error_interface = {.get_description =
                                               ut_gif_error_get_description};

static UtObjectInterface object_interface = {
    .type_name = "UtGifError",
    .to_string = ut_gif_error_to_string,
    .cleanup = ut_gif_error_cleanup,
    .interfaces = {{&ut_error_id, &error_interface}, {NULL, NULL}}};

UtObject *ut_gif_error_new(const char *description) {
  UtObject *object = ut_object_new(sizeof(UtGifError), &object_interface);
  UtGifError *self = (UtGifError *)object;
  self->description = ut_cstring_new(description);
  return object;
}

bool ut_object_is_gif_error(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
