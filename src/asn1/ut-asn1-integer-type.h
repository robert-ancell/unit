#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1IntegerType] object.
///
/// !return-ref
UtObject *ut_asn1_integer_type_new();

/// Returns [true] if [object] is a [UtAsn1IntegerType].
bool ut_object_is_asn1_integer_type(UtObject *object);
