#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtProtobufMessageFieldType type;
  UtObject *value_type;
  char *name;
  uint32_t number;
} UtProtobufMessageField;

static char *ut_protobuf_message_field_to_string(UtObject *object) {
  UtProtobufMessageField *self = (UtProtobufMessageField *)object;
  ut_cstring_ref value_type_string = ut_object_to_string(self->value_type);
  return ut_cstring_new_printf("<UtProtobufMessageField>(%s, %s, %u)",
                               value_type_string, self->name, self->number);
}

static void ut_protobuf_message_field_cleanup(UtObject *object) {
  UtProtobufMessageField *self = (UtProtobufMessageField *)object;
  ut_object_unref(self->value_type);
  free(self->name);
}

static UtObjectInterface object_interface = {
    .type_name = "UtProtobufMessageField",
    .to_string = ut_protobuf_message_field_to_string,
    .cleanup = ut_protobuf_message_field_cleanup};

static UtObject *message_field_new(UtProtobufMessageFieldType type,
                                   UtObject *value_type, const char *name,
                                   uint32_t number) {
  UtObject *object =
      ut_object_new(sizeof(UtProtobufMessageField), &object_interface);
  UtProtobufMessageField *self = (UtProtobufMessageField *)object;

  self->type = type;
  self->value_type = ut_object_ref(value_type);
  self->name = ut_cstring_new(name);
  self->number = number;

  return object;
}

UtObject *ut_protobuf_message_field_new(UtObject *value_type, const char *name,
                                        uint32_t number) {
  return message_field_new(UT_PROTOBUF_MESSAGE_FIELD_TYPE_SINGLE, value_type,
                           name, number);
}

UtObject *ut_protobuf_message_field_new_optional(UtObject *value_type,
                                                 const char *name,
                                                 uint32_t number) {
  return message_field_new(UT_PROTOBUF_MESSAGE_FIELD_TYPE_OPTIONAL, value_type,
                           name, number);
}

UtObject *ut_protobuf_message_field_new_repeated(UtObject *value_type,
                                                 const char *name,
                                                 uint32_t number) {
  return message_field_new(UT_PROTOBUF_MESSAGE_FIELD_TYPE_REPEATED, value_type,
                           name, number);
}

UtObject *ut_protobuf_message_field_new_take(UtObject *value_type,
                                             const char *name,
                                             uint32_t number) {
  UtObject *object = ut_protobuf_message_field_new(value_type, name, number);
  ut_object_unref(value_type);
  return object;
}

UtObject *ut_protobuf_message_field_new_optional_take(UtObject *value_type,
                                                      const char *name,
                                                      uint32_t number) {
  UtObject *object =
      ut_protobuf_message_field_new_optional(value_type, name, number);
  ut_object_unref(value_type);
  return object;
}

UtObject *ut_protobuf_message_field_new_repeated_take(UtObject *value_type,
                                                      const char *name,
                                                      uint32_t number) {
  UtObject *object =
      ut_protobuf_message_field_new_repeated(value_type, name, number);
  ut_object_unref(value_type);
  return object;
}

UtProtobufMessageFieldType
ut_protobuf_message_field_get_type(UtObject *object) {
  assert(ut_object_is_protobuf_message_field(object));
  UtProtobufMessageField *self = (UtProtobufMessageField *)object;
  return self->type;
}

UtObject *ut_protobuf_message_field_get_value_type(UtObject *object) {
  assert(ut_object_is_protobuf_message_field(object));
  UtProtobufMessageField *self = (UtProtobufMessageField *)object;
  return self->value_type;
}

const char *ut_protobuf_message_field_get_name(UtObject *object) {
  assert(ut_object_is_protobuf_message_field(object));
  UtProtobufMessageField *self = (UtProtobufMessageField *)object;
  return self->name;
}

uint32_t ut_protobuf_message_field_get_number(UtObject *object) {
  assert(ut_object_is_protobuf_message_field(object));
  UtProtobufMessageField *self = (UtProtobufMessageField *)object;
  return self->number;
}

bool ut_object_is_protobuf_message_field(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
