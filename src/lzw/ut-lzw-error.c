#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  char *description;
} UtLzwError;

static char *ut_lzw_error_to_string(UtObject *object) {
  UtLzwError *self = (UtLzwError *)object;
  return ut_cstring_new_printf("<UtLzwError>(\"%s\")", self->description);
}

static void ut_lzw_error_cleanup(UtObject *object) {
  UtLzwError *self = (UtLzwError *)object;
  free(self->description);
}

static char *ut_lzw_error_get_description(UtObject *object) {
  UtLzwError *self = (UtLzwError *)object;
  return ut_cstring_new(self->description);
}

static UtErrorInterface error_interface = {.get_description =
                                               ut_lzw_error_get_description};

static UtObjectInterface object_interface = {
    .type_name = "UtLzwError",
    .to_string = ut_lzw_error_to_string,
    .cleanup = ut_lzw_error_cleanup,
    .interfaces = {{&ut_error_id, &error_interface}, {NULL, NULL}}};

UtObject *ut_lzw_error_new(const char *description) {
  UtObject *object = ut_object_new(sizeof(UtLzwError), &object_interface);
  UtLzwError *self = (UtLzwError *)object;
  self->description = ut_cstring_new(description);
  return object;
}

bool ut_object_is_lzw_error(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
