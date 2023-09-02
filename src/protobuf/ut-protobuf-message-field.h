#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef enum {
  UT_PROTOBUF_MESSAGE_FIELD_TYPE_SINGLE,
  UT_PROTOBUF_MESSAGE_FIELD_TYPE_OPTIONAL,
  UT_PROTOBUF_MESSAGE_FIELD_TYPE_REPEATED
} UtProtobufMessageFieldType;

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new(UtObject *value_type, const char *name,
                                        uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_optional(UtObject *value_type,
                                                 const char *name,
                                                 uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_repeated(UtObject *value_type,
                                                 const char *name,
                                                 uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_take(UtObject *value_type,
                                             const char *name, uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_optional_take(UtObject *value_type,
                                                      const char *name,
                                                      uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_repeated_take(UtObject *value_type,
                                                      const char *name,
                                                      uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_double(const char *name,
                                               uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_optional_double(const char *name,
                                                        uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_repeated_double(const char *name,
                                                        uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_float(const char *name,
                                              uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_optional_float(const char *name,
                                                       uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_repeated_float(const char *name,
                                                       uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_int32(const char *name,
                                              uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_optional_int32(const char *name,
                                                       uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_repeated_int32(const char *name,
                                                       uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_int64(const char *name,
                                              uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_optional_int64(const char *name,
                                                       uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_repeated_int64(const char *name,
                                                       uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_uint32(const char *name,
                                               uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_optional_uint32(const char *name,
                                                        uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_repeated_uint32(const char *name,
                                                        uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_uint64(const char *name,
                                               uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_optional_uint64(const char *name,
                                                        uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_repeated_uint64(const char *name,
                                                        uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_sint32(const char *name,
                                               uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_optional_sint32(const char *name,
                                                        uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_repeated_sint32(const char *name,
                                                        uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_sint64(const char *name,
                                               uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_optional_sint64(const char *name,
                                                        uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_repeated_sint64(const char *name,
                                                        uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_fixed32(const char *name,
                                                uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_optional_fixed32(const char *name,
                                                         uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_repeated_fixed32(const char *name,
                                                         uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_fixed64(const char *name,
                                                uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_optional_fixed64(const char *name,
                                                         uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_repeated_fixed64(const char *name,
                                                         uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_sfixed32(const char *name,
                                                 uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_optional_sfixed32(const char *name,
                                                          uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_repeated_sfixed32(const char *name,
                                                          uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_sfixed64(const char *name,
                                                 uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_optional_sfixed64(const char *name,
                                                          uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_repeated_sfixed64(const char *name,
                                                          uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_bool(const char *name, uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_optional_bool(const char *name,
                                                      uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_repeated_bool(const char *name,
                                                      uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_string(const char *name,
                                               uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_optional_string(const char *name,
                                                        uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_repeated_string(const char *name,
                                                        uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_bytes(const char *name,
                                              uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_optional_bytes(const char *name,
                                                       uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_repeated_bytes(const char *name,
                                                       uint32_t number);

UtProtobufMessageFieldType ut_protobuf_message_field_get_type(UtObject *object);

UtObject *ut_protobuf_message_field_get_value_type(UtObject *object);

const char *ut_protobuf_message_field_get_name(UtObject *object);

uint32_t ut_protobuf_message_field_get_number(UtObject *object);

/// Returns [true] if [object] is a [UtProtobufMessageField].
bool ut_object_is_protobuf_message_field(UtObject *object);
