#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1OctetStringType] object.
///
/// !return-ref
UtObject *ut_asn1_octet_string_type_new();

/// Returns [true] if [object] is a [UtAsn1OctetStringType].
bool ut_object_is_asn1_octet_string_type(UtObject *object);
