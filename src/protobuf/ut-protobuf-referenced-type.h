#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new protobuf type reference with [name].
///
/// !return-ref
/// !return-type UtProtobufReferencedType
UtObject *ut_protobuf_referenced_type_new(const char *name);

/// Returns the name used in this reference.
const char *ut_protobuf_referenced_type_get_name(UtObject *object);

/// Set the [type] this reference points to.
///
/// !arg-type type UtProtobufPrimitiveType UtProtobufMessageType
/// UtProtobufEnumType
void ut_protobuf_referenced_type_set_type(UtObject *object, UtObject *type);

/// Returns the type this reference points to.
///
/// !return-type UtProtobufPrimitiveType UtProtobufMessageType
/// UtProtobufEnumType
UtObject *ut_protobuf_referenced_type_get_type(UtObject *object);

/// Returns [true] if [object] is a [UtProtobufReferencedType].
bool ut_object_is_protobuf_referenced_type(UtObject *object);
