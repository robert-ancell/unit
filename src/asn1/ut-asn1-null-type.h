#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1NullType] object.
///
/// !return-ref
UtObject *ut_asn1_null_type_new();

/// Returns [true] if [object] is a [UtAsn1NullType].
bool ut_object_is_asn1_null_type(UtObject *object);
