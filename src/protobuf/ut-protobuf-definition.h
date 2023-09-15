#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new protobuf definition.
///
/// !return-ref
/// !return-type UtProtobufDefinition
UtObject *ut_protobuf_definition_new(int syntax, UtObject *definitions);

/// Creates a new protobuf definition from [text].
///
/// !return-ref
/// !return-type UtProtobufDefinition
UtObject *ut_protobuf_definition_new_from_text(const char *text);

/// Returns the syntax this protobuf definition is using, either 2 or 3.
int ut_protobuf_definition_get_syntax(UtObject *object);

/// Returns the definitions.
UtObject *ut_protobuf_definition_get_definitions(UtObject *object);

/// Gets the definition called [name].
UtObject *ut_protobuf_definition_lookup(UtObject *object, const char *name);

/// Returns [true] if [object] is a [UtProtobufDefinition].
bool ut_object_is_protobuf_definition(UtObject *object);
