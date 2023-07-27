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

void ut_asn1_module_definition_set_exports(UtObject *object, UtObject *exports);

void ut_asn1_module_definition_set_export_all(UtObject *object);

void ut_asn1_module_definition_set_imports(UtObject *object, UtObject *imports);

/// Returns the identifier for this module.
const char *ut_asn1_module_definition_get_identifier(UtObject *object);

UtObject *ut_asn1_module_definition_get_object_identifier(UtObject *object);

UtObject *ut_asn1_module_definition_get_exports(UtObject *object);

bool ut_asn1_module_definition_get_export_all(UtObject *object);

UtObject *ut_asn1_module_definition_get_imports(UtObject *object);

/// Returns the assignments in this module.
UtObject *ut_asn1_module_definition_get_assignments(UtObject *object);

UtObject *ut_asn1_module_definition_get_assignment(UtObject *object,
                                                   const char *name);

/// Returns [true] if [object] is a [UtAsn1ModuleDefinition].
bool ut_object_is_asn1_module_definition(UtObject *object);
