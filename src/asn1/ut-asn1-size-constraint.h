#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 size constraint with [constraint].
///
/// !arg-type constraint UtObject
/// !return-ref
/// !return-type UtAsn1SizeConstraint
UtObject *ut_asn1_size_constraint_new(UtObject *constraint);

/// Returns the constraint that the size is limited to.
///
/// !return-type UtObject
UtObject *ut_asn1_size_constraint_get_constraint(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1SizeConstraint].
bool ut_object_is_asn1_size_constraint(UtObject *object);
