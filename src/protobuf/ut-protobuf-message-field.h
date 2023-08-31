#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new(UtObject *type, const char *name,
                                        uint32_t number);

/// Creates a new [UtProtobufMessageField] object.
///
/// !return-ref
UtObject *ut_protobuf_message_field_new_take(UtObject *type, const char *name,
                                             uint32_t number);

UtObject *ut_protobuf_message_field_get_type(UtObject *object);

const char *ut_protobuf_message_field_get_name(UtObject *object);

uint32_t ut_protobuf_message_field_get_number(UtObject *object);

/// Returns [true] if [object] is a [UtProtobufMessageField].
bool ut_object_is_protobuf_message_field(UtObject *object);