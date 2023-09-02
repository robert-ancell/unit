#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *request_type;
  UtObject *response_type;
} UtProtobufMethodCall;

static char *ut_protobuf_method_call_to_string(UtObject *object) {
  UtProtobufMethodCall *self = (UtProtobufMethodCall *)object;
  ut_cstring_ref request_type_string = ut_object_to_string(self->request_type);
  ut_cstring_ref response_type_string =
      ut_object_to_string(self->response_type);
  return ut_cstring_new_printf("<UtProtobufMethodCall>(%s, %s)",
                               request_type_string, response_type_string);
}

static void ut_protobuf_method_call_cleanup(UtObject *object) {
  UtProtobufMethodCall *self = (UtProtobufMethodCall *)object;
  ut_object_unref(self->request_type);
  ut_object_unref(self->response_type);
}

static UtObjectInterface object_interface = {
    .type_name = "UtProtobufMethodCall",
    .to_string = ut_protobuf_method_call_to_string,
    .cleanup = ut_protobuf_method_call_cleanup};

UtObject *ut_protobuf_method_call_new(UtObject *request_type,
                                      UtObject *response_type) {
  UtObject *object =
      ut_object_new(sizeof(UtProtobufMethodCall), &object_interface);
  UtProtobufMethodCall *self = (UtProtobufMethodCall *)object;

  self->request_type = ut_object_ref(request_type);
  self->response_type = ut_object_ref(response_type);

  return object;
}

UtObject *ut_protobuf_method_call_get_request_type(UtObject *object) {
  assert(ut_object_is_protobuf_method_call(object));
  UtProtobufMethodCall *self = (UtProtobufMethodCall *)object;
  return self->request_type;
}

UtObject *ut_protobuf_method_call_get_response_type(UtObject *object) {
  assert(ut_object_is_protobuf_method_call(object));
  UtProtobufMethodCall *self = (UtProtobufMethodCall *)object;
  return self->response_type;
}

bool ut_object_is_protobuf_method_call(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
