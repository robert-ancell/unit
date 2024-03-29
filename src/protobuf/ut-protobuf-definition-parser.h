#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new protobuf definition parser.
///
/// !return-ref
/// !return-type UtProtobufDefinitionParser
UtObject *ut_protobuf_definition_parser_new();

/// Parse [text] and return true if is a valid protobuf definition.
bool ut_protobuf_definition_parser_parse(UtObject *object, const char *text);

/// Gets the definition decoded.
///
/// !return-type UtProtobufDefinition
UtObject *ut_protobuf_definition_parser_get_definition(UtObject *object);

/// Gets any error that occurred during decoding.
///
/// !return-type UtProtobufError NULL
UtObject *ut_protobuf_definition_parser_get_error(UtObject *object);

/// Returns [true] if [object] is a [UtProtobufDefinitionParser].
bool ut_object_is_protobuf_definition_parser(UtObject *object);
