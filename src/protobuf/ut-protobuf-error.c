#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  char *description;
} UtProtobufError;

static char *ut_protobuf_error_to_string(UtObject *object) {
  UtProtobufError *self = (UtProtobufError *)object;
  return ut_cstring_new_printf("<UtProtobufError>(\"%s\")", self->description);
}

static void ut_protobuf_error_cleanup(UtObject *object) {
  UtProtobufError *self = (UtProtobufError *)object;
  free(self->description);
}

static char *ut_protobuf_error_get_description(UtObject *object) {
  UtProtobufError *self = (UtProtobufError *)object;
  return ut_cstring_new(self->description);
}

static UtErrorInterface error_interface = {
    .get_description = ut_protobuf_error_get_description};

static UtObjectInterface object_interface = {
    .type_name = "UtProtobufError",
    .to_string = ut_protobuf_error_to_string,
    .cleanup = ut_protobuf_error_cleanup,
    .interfaces = {{&ut_error_id, &error_interface}, {NULL, NULL}}};

UtObject *ut_protobuf_error_new(const char *description) {
  UtObject *object = ut_object_new(sizeof(UtProtobufError), &object_interface);
  UtProtobufError *self = (UtProtobufError *)object;
  self->description = ut_cstring_new(description);
  return object;
}

bool ut_object_is_protobuf_error(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
