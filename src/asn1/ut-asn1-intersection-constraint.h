#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1IntersectionConstraint] object.
///
/// !return-ref
UtObject *ut_asn1_intersection_constraint_new(UtObject *constraints);

UtObject *ut_asn1_intersection_constraint_get_constraints(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1IntersectionConstraint].
bool ut_object_is_asn1_intersection_constraint(UtObject *object);
