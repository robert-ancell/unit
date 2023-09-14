#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 REAL type.
///
/// !return-ref
/// !return-type UtAsn1RealType
UtObject *ut_asn1_real_type_new();

/// Returns [true] if [object] is a [UtAsn1RealType].
bool ut_object_is_asn1_real_type(UtObject *object);
