#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 BMPString type.
///
/// !return-ref
/// !return-type UtAsn1BmpStringType
UtObject *ut_asn1_bmp_string_type_new();

/// Returns [true] if [object] is a [UtAsn1BmpStringType].
bool ut_object_is_asn1_bmp_string_type(UtObject *object);
