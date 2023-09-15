#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 intersection constraint made up of [constraints].
///
/// !arg-type constraints UtObjectList
/// !return-ref
/// !return-type UtAsn1IntersectionConstraint
UtObject *ut_asn1_intersection_constraint_new(UtObject *constraints);

/// Returns the constraints that make up this intersection.
///
/// !return-type UtObjectList
UtObject *ut_asn1_intersection_constraint_get_constraints(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1IntersectionConstraint].
bool ut_object_is_asn1_intersection_constraint(UtObject *object);
