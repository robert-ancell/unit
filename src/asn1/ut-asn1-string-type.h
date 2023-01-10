#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1StringType] object.
///
/// !return-ref
UtObject *ut_asn1_string_type_new();

/// Returns [true] if [object] is a [UtAsn1StringType].
bool ut_object_is_asn1_string_type(UtObject *object);
