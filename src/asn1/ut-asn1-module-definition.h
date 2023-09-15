#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 module definition.
/// The module requires an [identifier] and an options [object_identifier].
/// Modules contain type and value [assignments].
///
/// !arg-type object_identifier UtObjectIdentifier NULL
/// !arg-type assignments UtMap
/// !return-ref
/// !return-type UtAsn1ModuleDefinition
UtObject *ut_asn1_module_definition_new(const char *identifier,
                                        UtObject *object_identifier,
                                        UtObject *assignments);

/// Creates a new ASN.1 module definition from [text].
///
/// !return-ref
/// !return-type UtAsn1ModuleDefinition
UtObject *ut_asn1_module_definition_new_from_text(const char *text);

/// Returns the identifier for this module.
const char *ut_asn1_module_definition_get_identifier(UtObject *object);

/// Retunrn the object identifier for this module or [NULL] if none set.
///
/// !return-type UtObjectIdentifier NULL
UtObject *ut_asn1_module_definition_get_object_identifier(UtObject *object);

/// Returns the assignments in this module.
UtObject *ut_asn1_module_definition_get_assignments(UtObject *object);

/// Returns the assignment with [name] or [NULL] if no assignment is set.
///
/// !return-type UtAsn1Type UtAsn1Value
UtObject *ut_asn1_module_definition_lookup_assignment(UtObject *object,
                                                      const char *name);

/// Returns [true] if [object] is a [UtAsn1ModuleDefinition].
bool ut_object_is_asn1_module_definition(UtObject *object);
