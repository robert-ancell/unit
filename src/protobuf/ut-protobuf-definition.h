#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new protobuf definition using [syntax] (2 or 3) and containing
/// [definitions].
///
/// !arg-type definitions UtMap
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
///
/// !return-type UtMap
UtObject *ut_protobuf_definition_get_definitions(UtObject *object);

/// Gets the definition called [name].
///
/// !return-type UtProtobufMessageType
/// !return-type UtProtobufEnumType
/// !return-type UtProtobufService
/// !return-type NULL
UtObject *ut_protobuf_definition_lookup(UtObject *object, const char *name);

/// Returns [true] if [object] is a [UtProtobufDefinition].
bool ut_object_is_protobuf_definition(UtObject *object);
