#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1ModuleImports] object.
///
/// !return-ref
UtObject *ut_asn1_module_imports_new();

/// Returns the module that is being imported from.
const char *ut_asn1_module_imports_get_module(UtObject *object);

/// Returns the names of the symbols that are being imported.
UtObject *ut_asn1_module_imports_get_symbols(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1ModuleImports].
bool ut_object_is_asn1_module_imports(UtObject *object);
