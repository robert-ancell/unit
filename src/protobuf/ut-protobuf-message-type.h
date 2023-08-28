#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtProtobufMessageType] object.
///
/// !return-ref
UtObject *ut_protobuf_message_type_new(UtObject *fields);

/// Creates a new [UtProtobufMessageType] object.
///
/// !return-ref
UtObject *ut_protobuf_message_type_new_take(UtObject *fields);

UtObject *ut_protobuf_message_type_get_fields(UtObject *object);

UtObject *ut_protobuf_message_type_get_field(UtObject *object,
                                             const char *name);

/// Returns [true] if [object] is a [UtProtobufMessageType].
bool ut_object_is_protobuf_message_type(UtObject *object);
