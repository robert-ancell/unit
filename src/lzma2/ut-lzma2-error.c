#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  char *description;
} UtLzma2Error;

static char *ut_lzma2_error_to_string(UtObject *object) {
  UtLzma2Error *self = (UtLzma2Error *)object;
  return ut_cstring_new_printf("<UtLzma2Error>(\"%s\")", self->description);
}

static void ut_lzma2_error_cleanup(UtObject *object) {
  UtLzma2Error *self = (UtLzma2Error *)object;
  free(self->description);
}

static char *ut_lzma2_error_get_description(UtObject *object) {
  UtLzma2Error *self = (UtLzma2Error *)object;
  return ut_cstring_new(self->description);
}

static UtErrorInterface error_interface = {.get_description =
                                               ut_lzma2_error_get_description};

static UtObjectInterface object_interface = {
    .type_name = "UtLzma2Error",
    .to_string = ut_lzma2_error_to_string,
    .cleanup = ut_lzma2_error_cleanup,
    .interfaces = {{&ut_error_id, &error_interface}, {NULL, NULL}}};

UtObject *ut_lzma2_error_new(const char *description) {
  UtObject *object = ut_object_new(sizeof(UtLzma2Error), &object_interface);
  UtLzma2Error *self = (UtLzma2Error *)object;
  self->description = ut_cstring_new(description);
  return object;
}

bool ut_object_is_lzma2_error(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
