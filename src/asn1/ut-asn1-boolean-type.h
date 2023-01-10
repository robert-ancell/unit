#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1BooleanType] object.
///
/// !return-ref
UtObject *ut_asn1_boolean_type_new();

/// Returns [true] if [object] is a [UtAsn1BooleanType].
bool ut_object_is_asn1_boolean_type(UtObject *object);
