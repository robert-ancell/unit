#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1GeneralizedTimeType] object.
///
/// !return-ref
UtObject *ut_asn1_generalized_time_type_new();

/// Returns [true] if [object] is a [UtAsn1GeneralizedTimeType].
bool ut_object_is_asn1_generalized_time_type(UtObject *object);
