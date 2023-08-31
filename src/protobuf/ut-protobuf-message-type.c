#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *fields;
} UtProtobufMessageType;

static char *ut_protobuf_message_type_to_string(UtObject *object) {
  UtProtobufMessageType *self = (UtProtobufMessageType *)object;
  ut_cstring_ref fields_string = ut_object_to_string(self->fields);
  return ut_cstring_new_printf("<UtProtobufMessageType>(%s)", fields_string);
}

static void ut_protobuf_message_type_cleanup(UtObject *object) {
  UtProtobufMessageType *self = (UtProtobufMessageType *)object;
  ut_object_unref(self->fields);
}

static UtObjectInterface object_interface = {
    .type_name = "UtProtobufMessageType",
    .to_string = ut_protobuf_message_type_to_string,
    .cleanup = ut_protobuf_message_type_cleanup};

UtObject *ut_protobuf_message_type_new(UtObject *fields) {
  UtObject *object =
      ut_object_new(sizeof(UtProtobufMessageType), &object_interface);
  UtProtobufMessageType *self = (UtProtobufMessageType *)object;

  self->fields = ut_object_ref(fields);

  return object;
}

UtObject *ut_protobuf_message_type_new_take(UtObject *fields) {
  UtObject *object = ut_protobuf_message_type_new(fields);
  ut_object_unref(fields);
  return object;
}

UtObject *ut_protobuf_message_type_get_fields(UtObject *object) {
  assert(ut_object_is_protobuf_message_type(object));
  UtProtobufMessageType *self = (UtProtobufMessageType *)object;
  return self->fields;
}

UtObject *ut_protobuf_message_type_get_field_by_name(UtObject *object,
                                                     const char *name) {
  assert(ut_object_is_protobuf_message_type(object));
  UtProtobufMessageType *self = (UtProtobufMessageType *)object;

  size_t fields_length = ut_list_get_length(self->fields);
  for (size_t i = 0; i < fields_length; i++) {
    UtObject *field = ut_object_list_get_element(self->fields, i);
    if (ut_cstring_equal(ut_protobuf_message_field_get_name(field), name)) {
      return field;
    }
  }

  return NULL;
}

UtObject *ut_protobuf_message_type_get_field_by_number(UtObject *object,
                                                       uint32_t number) {
  assert(ut_object_is_protobuf_message_type(object));
  UtProtobufMessageType *self = (UtProtobufMessageType *)object;

  size_t fields_length = ut_list_get_length(self->fields);
  for (size_t i = 0; i < fields_length; i++) {
    UtObject *field = ut_object_list_get_element(self->fields, i);
    if (ut_protobuf_message_field_get_number(field) == number) {
      return field;
    }
  }

  return NULL;
}

bool ut_object_is_protobuf_message_type(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
