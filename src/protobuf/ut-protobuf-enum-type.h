#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtProtobufEnumType] object.
///
/// !return-ref
UtObject *ut_protobuf_enum_type_new(UtObject *values_by_name);

/// Creates a new [UtProtobufEnumType] object.
///
/// !return-ref
UtObject *ut_protobuf_enum_type_new_take(UtObject *values_by_name);

bool ut_protobuf_enum_type_get_value(UtObject *object, const char *name,
                                     int32_t *value);

char *ut_protobuf_enum_type_get_name(UtObject *object, int32_t value);

/// Returns [true] if [object] is a [UtProtobufEnumType].
bool ut_object_is_protobuf_enum_type(UtObject *object);
