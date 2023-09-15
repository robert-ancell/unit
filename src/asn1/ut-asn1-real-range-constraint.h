#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 REAL range constraing from [lower] to [upper].
///
/// !return-ref
/// !return-type UtAsn1RealRangeConstraint
UtObject *ut_asn1_real_range_constraint_new(double lower, double upper);

/// Returns the lower bounds of this constraint.
double ut_asn1_real_range_constraint_get_lower(UtObject *object);

/// Returns the upper bounds of this constraint.
double ut_asn1_real_range_constraint_get_upper(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1RealRangeConstraint].
bool ut_object_is_asn1_real_range_constraint(UtObject *object);
