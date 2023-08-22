#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1RealRangeConstraint] object.
///
/// !return-ref
UtObject *ut_asn1_real_range_constraint_new(double lower, double upper);

double ut_asn1_real_range_constraint_get_lower(UtObject *object);

double ut_asn1_real_range_constraint_get_upper(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1RealRangeConstraint].
bool ut_object_is_asn1_real_range_constraint(UtObject *object);
