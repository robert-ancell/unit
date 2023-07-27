#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1VideotexStringType] object.
///
/// !return-ref
UtObject *ut_asn1_videotex_string_type_new();

/// Returns [true] if [object] is a [UtAsn1VideotexStringType].
bool ut_object_is_asn1_videotex_string_type(UtObject *object);
