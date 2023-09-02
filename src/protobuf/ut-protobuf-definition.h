#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtProtobufDefinition].
///
/// !return-ref
UtObject *ut_protobuf_definition_new(int syntax, UtObject *definitions);

/// Creates a new [UtProtobufDefinition] from [text].
///
/// !return-ref
UtObject *ut_protobuf_definition_new_from_text(const char *text);

int ut_protobuf_definition_get_syntax(UtObject *object);

/// Returns the definitions.
UtObject *ut_protobuf_definition_get_definitions(UtObject *object);

/// Gets the definition called [name].
UtObject *ut_protobuf_definition_lookup(UtObject *object, const char *name);

/// Returns [true] if [object] is a [UtProtobufDefinition].
bool ut_object_is_protobuf_definition(UtObject *object);
