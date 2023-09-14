#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new protobuf enum type containing [values_by_name].
///
/// !arg-type values_by_name UtMap
/// !return-ref
/// !return-type UtProtobufEnumType
UtObject *ut_protobuf_enum_type_new(UtObject *values_by_name);

/// Creates a new protobuf enum type and takes a reference to [values_by_name].
///
/// !take_ref values_by_name
/// !arg-type values_by_name UtMap
/// !return-ref
/// !return-type UtProtobufEnumType
UtObject *ut_protobuf_enum_type_new_take(UtObject *values_by_name);

/// Returns the values in this enum.
///
/// !return-type UtMap
UtObject *ut_protobuf_enum_type_lookup_values_by_name(UtObject *object);

/// Returns true if [name] is a valid value in this enum.
/// The value is written to [value].
bool ut_protobuf_enum_type_lookup_value(UtObject *object, const char *name,
                                        int32_t *value);

/// Returns the first name that matches [value] or [NULL] if no match.
char *ut_protobuf_enum_type_lookup_name(UtObject *object, int32_t value);

/// Returns [true] if [object] is a [UtProtobufEnumType].
bool ut_object_is_protobuf_enum_type(UtObject *object);
