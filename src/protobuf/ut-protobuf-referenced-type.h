#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtProtobufReferencedType] object.
///
/// !return-ref
UtObject *ut_protobuf_referenced_type_new(const char *name);

const char *ut_protobuf_referenced_type_get_name(UtObject *object);

void ut_protobuf_referenced_type_set_type(UtObject *object, UtObject *type);

UtObject *ut_protobuf_referenced_type_get_type(UtObject *object);

/// Returns [true] if [object] is a [UtProtobufReferencedType].
bool ut_object_is_protobuf_referenced_type(UtObject *object);
