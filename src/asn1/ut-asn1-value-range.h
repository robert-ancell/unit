#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1ValueRange] object.
///
/// !return-ref
UtObject *ut_asn1_value_range_new(UtObject *lower_value, UtObject *upper_value);

UtObject *ut_asn1_value_range_get_lower_value(UtObject *object);

UtObject *ut_asn1_value_range_get_upper_value(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1ValueRange].
bool ut_object_is_asn1_value_range(UtObject *object);
