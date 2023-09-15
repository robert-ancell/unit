#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 module definition parser.
///
/// !return-ref
/// !return-type UtAsn1ModuleDefinitionParser
UtObject *ut_asn1_module_definition_parser_new();

/// Parse [text] into a module definition.
bool ut_asn1_module_definition_parser_parse(UtObject *object, const char *text);

/// Returns the module definition decoded or [NULL] if none decoded.
///
/// !return-type UtAsn1ModuleDefinition NULL
UtObject *
ut_asn1_module_definition_parser_get_module_definition(UtObject *object);

/// Returns the first error that occurred during decoding or [NULL] if no error
/// occurred.
///
/// !return-type UtAsn1Error NULL
UtObject *ut_asn1_module_definition_parser_get_error(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1ModuleDefinitionParser].
bool ut_object_is_asn1_module_definition_parser(UtObject *object);
