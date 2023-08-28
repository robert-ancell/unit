#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtProtobufPrimitive type;
} UtProtobufPrimitiveType;

static UtObjectInterface object_interface = {.type_name =
                                                 "UtProtobufPrimitiveType"};

UtObject *ut_protobuf_primitive_type_new(UtProtobufPrimitive type) {
  UtObject *object =
      ut_object_new(sizeof(UtProtobufPrimitiveType), &object_interface);
  UtProtobufPrimitiveType *self = (UtProtobufPrimitiveType *)object;
  self->type = type;
  return object;
}

UtObject *ut_protobuf_primitive_type_new_double() {
  return ut_protobuf_primitive_type_new(UT_PROTOBUF_PRIMITIVE_DOUBLE);
}

UtObject *ut_protobuf_primitive_type_new_float() {
  return ut_protobuf_primitive_type_new(UT_PROTOBUF_PRIMITIVE_FLOAT);
}

UtObject *ut_protobuf_primitive_type_new_int32() {
  return ut_protobuf_primitive_type_new(UT_PROTOBUF_PRIMITIVE_INT32);
}

UtObject *ut_protobuf_primitive_type_new_int64() {
  return ut_protobuf_primitive_type_new(UT_PROTOBUF_PRIMITIVE_INT64);
}

UtObject *ut_protobuf_primitive_type_new_uint32() {
  return ut_protobuf_primitive_type_new(UT_PROTOBUF_PRIMITIVE_UINT32);
}

UtObject *ut_protobuf_primitive_type_new_uint64() {
  return ut_protobuf_primitive_type_new(UT_PROTOBUF_PRIMITIVE_UINT64);
}

UtObject *ut_protobuf_primitive_type_new_sint32() {
  return ut_protobuf_primitive_type_new(UT_PROTOBUF_PRIMITIVE_SINT32);
}

UtObject *ut_protobuf_primitive_type_new_sint64() {
  return ut_protobuf_primitive_type_new(UT_PROTOBUF_PRIMITIVE_SINT64);
}

UtObject *ut_protobuf_primitive_type_new_fixed32() {
  return ut_protobuf_primitive_type_new(UT_PROTOBUF_PRIMITIVE_FIXED32);
}

UtObject *ut_protobuf_primitive_type_new_fixed64() {
  return ut_protobuf_primitive_type_new(UT_PROTOBUF_PRIMITIVE_FIXED64);
}

UtObject *ut_protobuf_primitive_type_new_sfixed32() {
  return ut_protobuf_primitive_type_new(UT_PROTOBUF_PRIMITIVE_SFIXED32);
}

UtObject *ut_protobuf_primitive_type_new_sfixed64() {
  return ut_protobuf_primitive_type_new(UT_PROTOBUF_PRIMITIVE_SFIXED64);
}

UtObject *ut_protobuf_primitive_type_new_bool() {
  return ut_protobuf_primitive_type_new(UT_PROTOBUF_PRIMITIVE_BOOL);
}

UtObject *ut_protobuf_primitive_type_new_string() {
  return ut_protobuf_primitive_type_new(UT_PROTOBUF_PRIMITIVE_STRING);
}

UtObject *ut_protobuf_primitive_type_new_bytes() {
  return ut_protobuf_primitive_type_new(UT_PROTOBUF_PRIMITIVE_BYTES);
}

UtProtobufPrimitive ut_protobuf_primitive_type_get_type(UtObject *object) {
  assert(ut_object_is_protobuf_primitive_type(object));
  UtProtobufPrimitiveType *self = (UtProtobufPrimitiveType *)object;
  return self->type;
}

bool ut_object_is_protobuf_primitive_type(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
