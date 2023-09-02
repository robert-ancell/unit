#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *method_calls;
} UtProtobufService;

static char *ut_protobuf_service_to_string(UtObject *object) {
  UtProtobufService *self = (UtProtobufService *)object;
  ut_cstring_ref method_calls_string = ut_object_to_string(self->method_calls);
  return ut_cstring_new_printf("<UtProtobufService>(%s)", method_calls_string);
}

static void ut_protobuf_service_cleanup(UtObject *object) {
  UtProtobufService *self = (UtProtobufService *)object;
  ut_object_unref(self->method_calls);
}

static UtObjectInterface object_interface = {
    .type_name = "UtProtobufService",
    .to_string = ut_protobuf_service_to_string,
    .cleanup = ut_protobuf_service_cleanup};

UtObject *ut_protobuf_service_new(UtObject *method_calls) {
  UtObject *object =
      ut_object_new(sizeof(UtProtobufService), &object_interface);
  UtProtobufService *self = (UtProtobufService *)object;

  self->method_calls = ut_object_ref(method_calls);

  return object;
}

UtObject *ut_protobuf_service_get_method_calls(UtObject *object) {
  assert(ut_object_is_protobuf_service(object));
  UtProtobufService *self = (UtProtobufService *)object;
  return self->method_calls;
}

UtObject *ut_protobuf_service_get_method_call(UtObject *object,
                                              const char *name) {
  assert(ut_object_is_protobuf_service(object));
  UtProtobufService *self = (UtProtobufService *)object;
  return ut_map_lookup_string(self->method_calls, name);
}

bool ut_object_is_protobuf_service(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
