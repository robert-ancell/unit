#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef enum {
  UT_PROTOBUF_PRIMITIVE_DOUBLE,
  UT_PROTOBUF_PRIMITIVE_FLOAT,
  UT_PROTOBUF_PRIMITIVE_INT32,
  UT_PROTOBUF_PRIMITIVE_INT64,
  UT_PROTOBUF_PRIMITIVE_UINT32,
  UT_PROTOBUF_PRIMITIVE_UINT64,
  UT_PROTOBUF_PRIMITIVE_SINT32,
  UT_PROTOBUF_PRIMITIVE_SINT64,
  UT_PROTOBUF_PRIMITIVE_FIXED32,
  UT_PROTOBUF_PRIMITIVE_FIXED64,
  UT_PROTOBUF_PRIMITIVE_SFIXED32,
  UT_PROTOBUF_PRIMITIVE_SFIXED64,
  UT_PROTOBUF_PRIMITIVE_BOOL,
  UT_PROTOBUF_PRIMITIVE_STRING,
  UT_PROTOBUF_PRIMITIVE_BYTES
} UtProtobufPrimitive;

/// Creates a new [UtProtobufPrimitiveType] object.
///
/// !return-ref
UtObject *ut_protobuf_primitive_type_new(UtProtobufPrimitive type);

/// !return-ref
UtObject *ut_protobuf_primitive_type_new_double();

/// !return-ref
UtObject *ut_protobuf_primitive_type_new_float();

/// !return-ref
UtObject *ut_protobuf_primitive_type_new_int32();

/// !return-ref
UtObject *ut_protobuf_primitive_type_new_int64();

/// !return-ref
UtObject *ut_protobuf_primitive_type_new_uint32();

/// !return-ref
UtObject *ut_protobuf_primitive_type_new_uint64();

/// !return-ref
UtObject *ut_protobuf_primitive_type_new_sint32();

/// !return-ref
UtObject *ut_protobuf_primitive_type_new_sint64();

/// !return-ref
UtObject *ut_protobuf_primitive_type_new_fixed32();

/// !return-ref
UtObject *ut_protobuf_primitive_type_new_fixed64();

/// !return-ref
UtObject *ut_protobuf_primitive_type_new_sfixed32();

/// !return-ref
UtObject *ut_protobuf_primitive_type_new_sfixed64();

/// !return-ref
UtObject *ut_protobuf_primitive_type_new_bool();

/// !return-ref
UtObject *ut_protobuf_primitive_type_new_string();

/// !return-ref
UtObject *ut_protobuf_primitive_type_new_bytes();

UtProtobufPrimitive ut_protobuf_primitive_type_get_type(UtObject *object);

/// Returns [true] if [object] is a [UtProtobufPrimitiveType].
bool ut_object_is_protobuf_primitive_type(UtObject *object);
