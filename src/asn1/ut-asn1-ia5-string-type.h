#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 IA5String type.
///
/// !return-ref
/// !return-type UtAsn1Ia5StringType
UtObject *ut_asn1_ia5_string_type_new();

/// Returns [true] if [object] is a [UtAsn1Ia5StringType].
bool ut_object_is_asn1_ia5_string_type(UtObject *object);
