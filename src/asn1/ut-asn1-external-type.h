#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1ExternalType] object.
///
/// !return-ref
UtObject *ut_asn1_external_type_new();

/// Returns [true] if [object] is a [UtAsn1ExternalType].
bool ut_object_is_asn1_external_type(UtObject *object);
