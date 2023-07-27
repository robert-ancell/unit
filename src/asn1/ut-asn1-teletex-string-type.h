#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1TeletexStringType] object.
///
/// !return-ref
UtObject *ut_asn1_teletex_string_type_new();

/// Returns [true] if [object] is a [UtAsn1TeletexStringType].
bool ut_object_is_asn1_teletex_string_type(UtObject *object);
