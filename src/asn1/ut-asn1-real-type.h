#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1RealType] object.
///
/// !return-ref
UtObject *ut_asn1_real_type_new();

/// Returns [true] if [object] is a [UtAsn1RealType].
bool ut_object_is_asn1_real_type(UtObject *object);
