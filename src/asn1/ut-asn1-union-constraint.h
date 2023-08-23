#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1UnionConstraint] object.
///
/// !return-ref
UtObject *ut_asn1_union_constraint_new(UtObject *constraints);

UtObject *ut_asn1_union_constraint_get_constraints(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1UnionConstraint].
bool ut_object_is_asn1_union_constraint(UtObject *object);
