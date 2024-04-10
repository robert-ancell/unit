#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  char *description;
} UtTvgError;

static char *ut_tvg_error_to_string(UtObject *object) {
  UtTvgError *self = (UtTvgError *)object;
  return ut_cstring_new_printf("<UtTvgError>(\"%s\")", self->description);
}

static void ut_tvg_error_cleanup(UtObject *object) {
  UtTvgError *self = (UtTvgError *)object;
  free(self->description);
}

static char *ut_tvg_error_get_description(UtObject *object) {
  UtTvgError *self = (UtTvgError *)object;
  return ut_cstring_new(self->description);
}

static UtErrorInterface error_interface = {.get_description =
                                               ut_tvg_error_get_description};

static UtObjectInterface object_interface = {
    .type_name = "UtTvgError",
    .to_string = ut_tvg_error_to_string,
    .cleanup = ut_tvg_error_cleanup,
    .interfaces = {{&ut_error_id, &error_interface}, {NULL, NULL}}};

UtObject *ut_tvg_error_new(const char *description) {
  UtObject *object = ut_object_new(sizeof(UtTvgError), &object_interface);
  UtTvgError *self = (UtTvgError *)object;
  self->description = ut_cstring_new(description);
  return object;
}

bool ut_object_is_tvg_error(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
