#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1ModuleDefinition].
///
/// !return-ref
UtObject *ut_asn1_module_definition_new(const char *identifier,
                                        UtObject *object_identifier,
                                        UtObject *assignments);

/// Creates a new [UtAsn1ModuleDefinition] from [text].
///
/// !return-ref
UtObject *ut_asn1_module_definition_new_from_text(const char *text);

/// Returns the identifier for this module.
const char *ut_asn1_module_definition_get_identifier(UtObject *object);

UtObject *ut_asn1_module_definition_get_object_identifier(UtObject *object);

/// Returns the assignments in this module.
UtObject *ut_asn1_module_definition_get_assignments(UtObject *object);

UtObject *ut_asn1_module_definition_get_assignment(UtObject *object,
                                                   const char *name);

/// Returns [true] if [object] is a [UtAsn1ModuleDefinition].
bool ut_object_is_asn1_module_definition(UtObject *object);
