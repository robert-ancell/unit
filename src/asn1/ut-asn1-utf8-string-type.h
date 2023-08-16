#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1Utf8StringType] object.
///
/// !return-ref
UtObject *ut_asn1_utf8_string_type_new();

/// Returns [true] if [object] is a [UtAsn1Utf8StringType].
bool ut_object_is_asn1_utf8_string_type(UtObject *object);
