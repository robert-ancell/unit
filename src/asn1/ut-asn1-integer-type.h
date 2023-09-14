#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 INTEGER type.
///
/// !return-ref
/// !return-type UtAsn1IntegerType
UtObject *ut_asn1_integer_type_new();

/// Returns [true] if [object] is a [UtAsn1IntegerType].
bool ut_object_is_asn1_integer_type(UtObject *object);
