#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Types of protobuf message fields.
/// - [UT_PROTOBUF_MESSAGE_FIELD_TYPE_IMPLICIT] - field is sent zero or more
/// times, and has a default value if not sent.
/// - [UT_PROTOBUF_MESSAGE_FIELD_TYPE_REQUIRED] - field is required to be sent
/// exactly once.
/// - [UT_PROTOBUF_MESSAGE_FIELD_TYPE_OPTIONAL] - field is sent zero or more
/// times, and has a default value if not sent. The client can tell if the field
/// was sent.
/// - [UT_PROTOBUF_MESSAGE_FIELD_TYPE_REPEATED] - field is sent zero or more
/// times, all values are stored in order received.
typedef enum {
  UT_PROTOBUF_MESSAGE_FIELD_TYPE_IMPLICIT,
  UT_PROTOBUF_MESSAGE_FIELD_TYPE_REQUIRED,
  UT_PROTOBUF_MESSAGE_FIELD_TYPE_OPTIONAL,
  UT_PROTOBUF_MESSAGE_FIELD_TYPE_REPEATED
} UtProtobufMessageFieldType;

/// Creates a new protobuf message field of [type] with [number] conatining
/// [value_type].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new(UtProtobufMessageFieldType type,
                                        UtObject *value_type, uint32_t number);

/// Creates a new protobuf optional message field with [number] containing
/// [value_type].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_optional(UtObject *value_type,
                                                 uint32_t number);

/// Creates a new protobuf repeated message field with [number] containing
/// [value_type].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_repeated(UtObject *value_type,
                                                 uint32_t number);

/// Creates a new protobuf implicit message field with [number] containing
/// [value_type].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_implicit_take(UtObject *value_type,
                                                      uint32_t number);

/// Creates a new protobuf optional message field with [number] containing
/// [value_type].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_optional_take(UtObject *value_type,
                                                      uint32_t number);

/// Creates a new protobuf repeated message field with [number] containing
/// [value_type].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_repeated_take(UtObject *value_type,
                                                      uint32_t number);

/// Creates a new protobuf message field of type `double` with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_double(uint32_t number);

/// Creates a new protobuf message field of type `optional double` with
/// [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_optional_double(uint32_t number);

/// Creates a new protobuf message field of type `repeated double` with
/// [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_repeated_double(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_float(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_optional_float(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_repeated_float(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_int32(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_optional_int32(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_repeated_int32(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_int64(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_optional_int64(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_repeated_int64(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_uint32(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_optional_uint32(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_repeated_uint32(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_uint64(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_optional_uint64(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_repeated_uint64(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_sint32(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_optional_sint32(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_repeated_sint32(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_sint64(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_optional_sint64(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_repeated_sint64(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_fixed32(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_optional_fixed32(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_repeated_fixed32(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_fixed64(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_optional_fixed64(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_repeated_fixed64(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_sfixed32(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_optional_sfixed32(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_repeated_sfixed32(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_sfixed64(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_optional_sfixed64(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_repeated_sfixed64(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_bool(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_optional_bool(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_repeated_bool(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_string(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_optional_string(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_repeated_string(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_bytes(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_optional_bytes(uint32_t number);

/// Creates a new protobuf message field with [number].
///
/// !return-ref
/// !return-type UtProtobufMessageField
UtObject *ut_protobuf_message_field_new_repeated_bytes(uint32_t number);

/// Returns the type of field this is.
UtProtobufMessageFieldType ut_protobuf_message_field_get_type(UtObject *object);

/// Returns the type this field is.
///
/// !return-type UtProtobufPrimitiveType UtProtobufMessageType
/// UtProtobufEnumType
UtObject *ut_protobuf_message_field_get_value_type(UtObject *object);

/// Returns the number of this field.
uint32_t ut_protobuf_message_field_get_number(UtObject *object);

/// Returns [true] if [object] is a [UtProtobufMessageField].
bool ut_object_is_protobuf_message_field(UtObject *object);
