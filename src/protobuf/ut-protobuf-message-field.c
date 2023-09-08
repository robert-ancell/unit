#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtProtobufMessageFieldType type;
  UtObject *value_type;
  uint32_t number;
} UtProtobufMessageField;

static char *ut_protobuf_message_field_to_string(UtObject *object) {
  UtProtobufMessageField *self = (UtProtobufMessageField *)object;
  ut_cstring_ref value_type_string = ut_object_to_string(self->value_type);
  return ut_cstring_new_printf("<UtProtobufMessageField>(%s, %u)",
                               value_type_string, self->number);
}

static void ut_protobuf_message_field_cleanup(UtObject *object) {
  UtProtobufMessageField *self = (UtProtobufMessageField *)object;
  ut_object_unref(self->value_type);
}

static UtObjectInterface object_interface = {
    .type_name = "UtProtobufMessageField",
    .to_string = ut_protobuf_message_field_to_string,
    .cleanup = ut_protobuf_message_field_cleanup};

static UtObject *message_field_new(UtProtobufMessageFieldType type,
                                   UtObject *value_type, uint32_t number) {
  UtObject *object =
      ut_object_new(sizeof(UtProtobufMessageField), &object_interface);
  UtProtobufMessageField *self = (UtProtobufMessageField *)object;

  self->type = type;
  self->value_type = ut_object_ref(value_type);
  self->number = number;

  return object;
}

UtObject *ut_protobuf_message_field_new(UtObject *value_type, uint32_t number) {
  return message_field_new(UT_PROTOBUF_MESSAGE_FIELD_TYPE_IMPLICIT, value_type,
                           number);
}

UtObject *ut_protobuf_message_field_new_optional(UtObject *value_type,

                                                 uint32_t number) {
  return message_field_new(UT_PROTOBUF_MESSAGE_FIELD_TYPE_OPTIONAL, value_type,
                           number);
}

UtObject *ut_protobuf_message_field_new_repeated(UtObject *value_type,

                                                 uint32_t number) {
  return message_field_new(UT_PROTOBUF_MESSAGE_FIELD_TYPE_REPEATED, value_type,
                           number);
}

UtObject *ut_protobuf_message_field_new_take(UtObject *value_type,

                                             uint32_t number) {
  UtObject *object = ut_protobuf_message_field_new(value_type, number);
  ut_object_unref(value_type);
  return object;
}

UtObject *ut_protobuf_message_field_new_optional_take(UtObject *value_type,

                                                      uint32_t number) {
  UtObject *object = ut_protobuf_message_field_new_optional(value_type, number);
  ut_object_unref(value_type);
  return object;
}

UtObject *ut_protobuf_message_field_new_repeated_take(UtObject *value_type,

                                                      uint32_t number) {
  UtObject *object = ut_protobuf_message_field_new_repeated(value_type, number);
  ut_object_unref(value_type);
  return object;
}

UtObject *ut_protobuf_message_field_new_double(uint32_t number) {
  return ut_protobuf_message_field_new_take(
      ut_protobuf_primitive_type_new_double(), number);
}

UtObject *ut_protobuf_message_field_new_optional_double(uint32_t number) {
  return ut_protobuf_message_field_new_optional_take(
      ut_protobuf_primitive_type_new_double(), number);
}

UtObject *ut_protobuf_message_field_new_repeated_double(uint32_t number) {
  return ut_protobuf_message_field_new_repeated_take(
      ut_protobuf_primitive_type_new_double(), number);
}

UtObject *ut_protobuf_message_field_new_float(uint32_t number) {
  return ut_protobuf_message_field_new_take(
      ut_protobuf_primitive_type_new_float(), number);
}

UtObject *ut_protobuf_message_field_new_optional_float(uint32_t number) {
  return ut_protobuf_message_field_new_optional_take(
      ut_protobuf_primitive_type_new_float(), number);
}

UtObject *ut_protobuf_message_field_new_repeated_float(uint32_t number) {
  return ut_protobuf_message_field_new_repeated_take(
      ut_protobuf_primitive_type_new_float(), number);
}

UtObject *ut_protobuf_message_field_new_int32(uint32_t number) {
  return ut_protobuf_message_field_new_take(
      ut_protobuf_primitive_type_new_int32(), number);
}

UtObject *ut_protobuf_message_field_new_optional_int32(uint32_t number) {
  return ut_protobuf_message_field_new_optional_take(
      ut_protobuf_primitive_type_new_int32(), number);
}

UtObject *ut_protobuf_message_field_new_repeated_int32(uint32_t number) {
  return ut_protobuf_message_field_new_repeated_take(
      ut_protobuf_primitive_type_new_int32(), number);
}

UtObject *ut_protobuf_message_field_new_int64(uint32_t number) {
  return ut_protobuf_message_field_new_take(
      ut_protobuf_primitive_type_new_int64(), number);
}

UtObject *ut_protobuf_message_field_new_optional_int64(uint32_t number) {
  return ut_protobuf_message_field_new_optional_take(
      ut_protobuf_primitive_type_new_int64(), number);
}

UtObject *ut_protobuf_message_field_new_repeated_int64(uint32_t number) {
  return ut_protobuf_message_field_new_repeated_take(
      ut_protobuf_primitive_type_new_int64(), number);
}

UtObject *ut_protobuf_message_field_new_uint32(uint32_t number) {
  return ut_protobuf_message_field_new_take(
      ut_protobuf_primitive_type_new_uint32(), number);
}

UtObject *ut_protobuf_message_field_new_optional_uint32(uint32_t number) {
  return ut_protobuf_message_field_new_optional_take(
      ut_protobuf_primitive_type_new_uint32(), number);
}

UtObject *ut_protobuf_message_field_new_repeated_uint32(uint32_t number) {
  return ut_protobuf_message_field_new_repeated_take(
      ut_protobuf_primitive_type_new_uint32(), number);
}

UtObject *ut_protobuf_message_field_new_uint64(uint32_t number) {
  return ut_protobuf_message_field_new_take(
      ut_protobuf_primitive_type_new_uint64(), number);
}

UtObject *ut_protobuf_message_field_new_optional_uint64(uint32_t number) {
  return ut_protobuf_message_field_new_optional_take(
      ut_protobuf_primitive_type_new_uint64(), number);
}

UtObject *ut_protobuf_message_field_new_repeated_uint64(uint32_t number) {
  return ut_protobuf_message_field_new_repeated_take(
      ut_protobuf_primitive_type_new_uint64(), number);
}

UtObject *ut_protobuf_message_field_new_sint32(uint32_t number) {
  return ut_protobuf_message_field_new_take(
      ut_protobuf_primitive_type_new_sint32(), number);
}

UtObject *ut_protobuf_message_field_new_optional_sint32(uint32_t number) {
  return ut_protobuf_message_field_new_optional_take(
      ut_protobuf_primitive_type_new_sint32(), number);
}

UtObject *ut_protobuf_message_field_new_repeated_sint32(uint32_t number) {
  return ut_protobuf_message_field_new_repeated_take(
      ut_protobuf_primitive_type_new_sint32(), number);
}

UtObject *ut_protobuf_message_field_new_sint64(uint32_t number) {
  return ut_protobuf_message_field_new_take(
      ut_protobuf_primitive_type_new_sint64(), number);
}

UtObject *ut_protobuf_message_field_new_optional_sint64(uint32_t number) {
  return ut_protobuf_message_field_new_optional_take(
      ut_protobuf_primitive_type_new_sint64(), number);
}

UtObject *ut_protobuf_message_field_new_repeated_sint64(uint32_t number) {
  return ut_protobuf_message_field_new_repeated_take(
      ut_protobuf_primitive_type_new_sint64(), number);
}

UtObject *ut_protobuf_message_field_new_fixed32(uint32_t number) {
  return ut_protobuf_message_field_new_take(
      ut_protobuf_primitive_type_new_fixed32(), number);
}

UtObject *ut_protobuf_message_field_new_optional_fixed32(uint32_t number) {
  return ut_protobuf_message_field_new_optional_take(
      ut_protobuf_primitive_type_new_fixed32(), number);
}

UtObject *ut_protobuf_message_field_new_repeated_fixed32(uint32_t number) {
  return ut_protobuf_message_field_new_repeated_take(
      ut_protobuf_primitive_type_new_fixed32(), number);
}

UtObject *ut_protobuf_message_field_new_fixed64(uint32_t number) {
  return ut_protobuf_message_field_new_take(
      ut_protobuf_primitive_type_new_fixed64(), number);
}

UtObject *ut_protobuf_message_field_new_optional_fixed64(uint32_t number) {
  return ut_protobuf_message_field_new_optional_take(
      ut_protobuf_primitive_type_new_fixed64(), number);
}

UtObject *ut_protobuf_message_field_new_repeated_fixed64(uint32_t number) {
  return ut_protobuf_message_field_new_repeated_take(
      ut_protobuf_primitive_type_new_fixed64(), number);
}

UtObject *ut_protobuf_message_field_new_sfixed32(uint32_t number) {
  return ut_protobuf_message_field_new_take(
      ut_protobuf_primitive_type_new_sfixed32(), number);
}

UtObject *ut_protobuf_message_field_new_optional_sfixed32(uint32_t number) {
  return ut_protobuf_message_field_new_optional_take(
      ut_protobuf_primitive_type_new_sfixed32(), number);
}

UtObject *ut_protobuf_message_field_new_repeated_sfixed32(uint32_t number) {
  return ut_protobuf_message_field_new_repeated_take(
      ut_protobuf_primitive_type_new_sfixed32(), number);
}

UtObject *ut_protobuf_message_field_new_sfixed64(uint32_t number) {
  return ut_protobuf_message_field_new_take(
      ut_protobuf_primitive_type_new_sfixed64(), number);
}

UtObject *ut_protobuf_message_field_new_optional_sfixed64(uint32_t number) {
  return ut_protobuf_message_field_new_optional_take(
      ut_protobuf_primitive_type_new_sfixed64(), number);
}

UtObject *ut_protobuf_message_field_new_repeated_sfixed64(uint32_t number) {
  return ut_protobuf_message_field_new_repeated_take(
      ut_protobuf_primitive_type_new_sfixed64(), number);
}

UtObject *ut_protobuf_message_field_new_bool(uint32_t number) {
  return ut_protobuf_message_field_new_take(
      ut_protobuf_primitive_type_new_bool(), number);
}

UtObject *ut_protobuf_message_field_new_optional_bool(uint32_t number) {
  return ut_protobuf_message_field_new_optional_take(
      ut_protobuf_primitive_type_new_bool(), number);
}

UtObject *ut_protobuf_message_field_new_repeated_bool(uint32_t number) {
  return ut_protobuf_message_field_new_repeated_take(
      ut_protobuf_primitive_type_new_bool(), number);
}

UtObject *ut_protobuf_message_field_new_string(uint32_t number) {
  return ut_protobuf_message_field_new_take(
      ut_protobuf_primitive_type_new_string(), number);
}

UtObject *ut_protobuf_message_field_new_optional_string(uint32_t number) {
  return ut_protobuf_message_field_new_optional_take(
      ut_protobuf_primitive_type_new_string(), number);
}

UtObject *ut_protobuf_message_field_new_repeated_string(uint32_t number) {
  return ut_protobuf_message_field_new_repeated_take(
      ut_protobuf_primitive_type_new_string(), number);
}

UtObject *ut_protobuf_message_field_new_bytes(uint32_t number) {
  return ut_protobuf_message_field_new_take(
      ut_protobuf_primitive_type_new_bytes(), number);
}

UtObject *ut_protobuf_message_field_new_optional_bytes(uint32_t number) {
  return ut_protobuf_message_field_new_optional_take(
      ut_protobuf_primitive_type_new_bytes(), number);
}

UtObject *ut_protobuf_message_field_new_repeated_bytes(uint32_t number) {
  return ut_protobuf_message_field_new_repeated_take(
      ut_protobuf_primitive_type_new_bytes(), number);
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

uint32_t ut_protobuf_message_field_get_number(UtObject *object) {
  assert(ut_object_is_protobuf_message_field(object));
  UtProtobufMessageField *self = (UtProtobufMessageField *)object;
  return self->number;
}

bool ut_object_is_protobuf_message_field(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
