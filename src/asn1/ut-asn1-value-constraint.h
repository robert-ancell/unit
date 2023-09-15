#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 constraint that limits to [value].
///
/// !arg-type value UtObject
/// !return-ref
/// !return-type UtAsn1ValueConstraint
UtObject *ut_asn1_value_constraint_new(UtObject *value);

/// Returns the value this constraint allows.
///
/// !return-type UtObject
UtObject *ut_asn1_value_constraint_get_value(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1ValueConstraint].
bool ut_object_is_asn1_value_constraint(UtObject *object);
