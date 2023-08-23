#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1RangeConstraint] object.
///
/// !return-ref
UtObject *ut_asn1_range_constraint_new(int64_t lower, int64_t upper);

int64_t ut_asn1_range_constraint_get_lower(UtObject *object);

int64_t ut_asn1_range_constraint_get_upper(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1RangeConstraint].
bool ut_object_is_asn1_range_constraint(UtObject *object);
