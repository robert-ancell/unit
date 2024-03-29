#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new protobuf message type containing [fields].
///
/// !arg-type fields UtObjectList
/// !return-ref
/// !return-type UtProtobufMessageType
UtObject *ut_protobuf_message_type_new(UtObject *fields);

/// Creates a new protobuf message type containing [fields].
///
/// !arg-type fields UtObjectList
/// !return-ref
/// !return-type UtProtobufMessageType
UtObject *ut_protobuf_message_type_new_take(UtObject *fields);

/// Returns the fields in this message type.
///
/// !return-type UtObjectList
UtObject *ut_protobuf_message_type_get_fields(UtObject *object);

/// Returns the field that matches [name] or [NULL] if no field.
///
/// !return-type UtProtobufMessageField NULL
UtObject *ut_protobuf_message_type_lookup_field(UtObject *object,
                                                const char *name);

/// Returns the field name that matches [number] or [NULL] if no field.
const char *ut_protobuf_message_type_lookup_field_name(UtObject *object,
                                                       uint32_t number);

/// Returns [true] if [object] is a [UtProtobufMessageType].
bool ut_object_is_protobuf_message_type(UtObject *object);
