#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1ModuleDefinitionParser] object.
///
/// !return-ref
UtObject *ut_asn1_module_definition_parser_new();

bool ut_asn1_module_definition_parser_parse(UtObject *object, const char *text);

/// Returns the module definition decoded.
UtObject *
ut_asn1_module_definition_parser_get_module_definition(UtObject *object);

UtObject *ut_asn1_module_definition_parser_get_error(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1ModuleDefinitionParser].
bool ut_object_is_asn1_module_definition_parser(UtObject *object);
