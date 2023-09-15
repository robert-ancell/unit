#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1SizeConstraint] object.
///
/// !return-ref
/// !return-type
UtObject *ut_asn1_size_constraint_new(UtObject *constraint);

UtObject *ut_asn1_size_constraint_get_constraint(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1SizeConstraint].
bool ut_object_is_asn1_size_constraint(UtObject *object);
