#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 BIT STRING type.
///
/// !return-ref
/// !return-type UtAsn1BitStringType
UtObject *ut_asn1_bit_string_type_new();

/// Returns [true] if [object] is a [UtAsn1BitStringType].
bool ut_object_is_asn1_bit_string_type(UtObject *object);
