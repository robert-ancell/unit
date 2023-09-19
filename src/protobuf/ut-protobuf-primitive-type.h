#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Primitive protobuf types:
/// - [UT_PROTOBUF_PRIMITIVE_DOUBLE] - double type.
/// - [UT_PROTOBUF_PRIMITIVE_FLOAT] - float type.
/// - [UT_PROTOBUF_PRIMITIVE_INT32] - int32 type.
/// - [UT_PROTOBUF_PRIMITIVE_INT64] - int64 type.
/// - [UT_PROTOBUF_PRIMITIVE_UINT32] - uint32 type.
/// - [UT_PROTOBUF_PRIMITIVE_UINT64] - uint64 type.
/// - [UT_PROTOBUF_PRIMITIVE_SINT32] - sint32 type.
/// - [UT_PROTOBUF_PRIMITIVE_SINT64] - sint64 type.
/// - [UT_PROTOBUF_PRIMITIVE_FIXED32] - fixed32 type.
/// - [UT_PROTOBUF_PRIMITIVE_FIXED64] - fixed64 type.
/// - [UT_PROTOBUF_PRIMITIVE_SFIXED32] - sfixed32 type.
/// - [UT_PROTOBUF_PRIMITIVE_SFIXED64] - sfixed64 type.
/// - [UT_PROTOBUF_PRIMITIVE_BOOL] - bool type.
/// - [UT_PROTOBUF_PRIMITIVE_STRING] - string type.
/// - [UT_PROTOBUF_PRIMITIVE_BYTES] - bytes type.
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

/// Creates a new protobuf primitive [type].
///
/// !return-ref
/// !return-type UtProtobufPrimitiveType
UtObject *ut_protobuf_primitive_type_new(UtProtobufPrimitive type);

/// Creates a new protobuf 'double' type.
///
/// !return-ref
/// !return-type UtProtobufPrimitiveType
UtObject *ut_protobuf_primitive_type_new_double();

/// Creates a new protobuf 'float' type.
///
/// !return-ref
/// !return-type UtProtobufPrimitiveType
UtObject *ut_protobuf_primitive_type_new_float();

/// Creates a new protobuf 'int32' type.
///
/// !return-ref
/// !return-type UtProtobufPrimitiveType
UtObject *ut_protobuf_primitive_type_new_int32();

/// Creates a new protobuf 'int64' type.
///
/// !return-ref
/// !return-type UtProtobufPrimitiveType
UtObject *ut_protobuf_primitive_type_new_int64();

/// Creates a new protobuf 'uint32' type.
///
/// !return-ref
/// !return-type UtProtobufPrimitiveType
UtObject *ut_protobuf_primitive_type_new_uint32();

/// Creates a new protobuf 'uint64' type.
///
/// !return-ref
/// !return-type UtProtobufPrimitiveType
UtObject *ut_protobuf_primitive_type_new_uint64();

/// Creates a new protobuf 'sint32' type.
///
/// !return-ref
/// !return-type UtProtobufPrimitiveType
UtObject *ut_protobuf_primitive_type_new_sint32();

/// Creates a new protobuf 'sint64' type.
///
/// !return-ref
/// !return-type UtProtobufPrimitiveType
UtObject *ut_protobuf_primitive_type_new_sint64();

/// Creates a new protobuf 'fixed32' type.
///
/// !return-ref
/// !return-type UtProtobufPrimitiveType
UtObject *ut_protobuf_primitive_type_new_fixed32();

/// Creates a new protobuf 'fixed64' type.
///
/// !return-ref
/// !return-type UtProtobufPrimitiveType
UtObject *ut_protobuf_primitive_type_new_fixed64();

/// Creates a new protobuf 'sfixed32' type.
///
/// !return-ref
/// !return-type UtProtobufPrimitiveType
UtObject *ut_protobuf_primitive_type_new_sfixed32();

/// Creates a new protobuf 'sfixed64' type.
///
/// !return-ref
/// !return-type UtProtobufPrimitiveType
UtObject *ut_protobuf_primitive_type_new_sfixed64();

/// Creates a new protobuf 'bool' type.
///
/// !return-ref
/// !return-type UtProtobufPrimitiveType
UtObject *ut_protobuf_primitive_type_new_bool();

/// Creates a new protobuf 'string' type.
///
/// !return-ref
/// !return-type UtProtobufPrimitiveType
UtObject *ut_protobuf_primitive_type_new_string();

/// Creates a new protobuf 'bytes' type.
///
/// !return-ref
/// !return-type UtProtobufPrimitiveType
UtObject *ut_protobuf_primitive_type_new_bytes();

/// Returns the protbuf type this represents.
UtProtobufPrimitive ut_protobuf_primitive_type_get_type(UtObject *object);

/// Returns [true] if [object] is a [UtProtobufPrimitiveType].
bool ut_object_is_protobuf_primitive_type(UtObject *object);
