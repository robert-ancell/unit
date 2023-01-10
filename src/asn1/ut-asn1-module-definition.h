#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1ModuleDefinition] object from [definition].
///
/// !return-ref
UtObject *ut_asn1_module_definition_new(const char *definition);

/// Returns the identifier for this module.
const char *ut_asn1_module_definition_get_identifier(UtObject *object);

/// Returns the type assignments in this module.
UtObject *ut_asn1_module_definition_get_type_assignments(UtObject *object);

/// Returns the value assignments in this module.
UtObject *ut_asn1_module_definition_get_value_assignments(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1ModuleDefinition].
bool ut_object_is_asn1_module_definition(UtObject *object);
