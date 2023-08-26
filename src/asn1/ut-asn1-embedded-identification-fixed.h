#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1EmbeddedIdentificationFixed] object.
///
/// !return-ref
UtObject *ut_asn1_embedded_identification_fixed_new();

/// Returns [true] if [object] is a [UtAsn1EmbeddedIdentificationFixed].
bool ut_object_is_asn1_embedded_identification_fixed(UtObject *object);
