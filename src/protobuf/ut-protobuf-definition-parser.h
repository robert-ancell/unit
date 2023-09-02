#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtProtobufDefinitionParser].
///
/// !return-ref
UtObject *ut_protobuf_definition_parser_new();

bool ut_protobuf_definition_parser_parse(UtObject *object, const char *text);

/// Gets the definition decoded.
UtObject *ut_protobuf_definition_parser_get_definition(UtObject *object);

/// Gets any error that occurred during decoding.
UtObject *ut_protobuf_definition_parser_get_error(UtObject *object);

/// Returns [true] if [object] is a [UtProtobufDefinitionParser].
bool ut_object_is_protobuf_definition_parser(UtObject *object);
