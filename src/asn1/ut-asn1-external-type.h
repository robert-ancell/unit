#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 EXTERNAL type.
///
/// !return-ref
/// !return-type UtAsn1ExternalType
UtObject *ut_asn1_external_type_new();

/// Returns [true] if [object] is a [UtAsn1ExternalType].
bool ut_object_is_asn1_external_type(UtObject *object);
