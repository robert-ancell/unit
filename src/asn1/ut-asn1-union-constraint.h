#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 union constraint made up of [constraints].
///
/// !arg-type constraints UtObjectList
/// !return-ref
/// !return-type UtAsn1UnionConstraint
UtObject *ut_asn1_union_constraint_new(UtObject *constraints);

/// Returns the constraints that make up this union.
///
/// !return-type UtObjectList
UtObject *ut_asn1_union_constraint_get_constraints(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1UnionConstraint].
bool ut_object_is_asn1_union_constraint(UtObject *object);
