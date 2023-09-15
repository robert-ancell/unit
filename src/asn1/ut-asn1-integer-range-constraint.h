#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 integer range constraing from [lower] to [upper].
///
/// !return-ref
/// !return-type UtAsn1IntegerRangeConstraint
UtObject *ut_asn1_integer_range_constraint_new(int64_t lower, int64_t upper);

/// Returns the lower bounds of this constraint.
int64_t ut_asn1_integer_range_constraint_get_lower(UtObject *object);

/// Returns the upper bounds of this constraint.
int64_t ut_asn1_integer_range_constraint_get_upper(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1IntegerRangeConstraint].
bool ut_object_is_asn1_integer_range_constraint(UtObject *object);
