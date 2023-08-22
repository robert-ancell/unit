#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1ValueConstraint] object.
///
/// !return-ref
UtObject *ut_asn1_value_constraint_new(UtObject *value);

UtObject *ut_asn1_value_constraint_get_value(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1ValueConstraint].
bool ut_object_is_asn1_value_constraint(UtObject *object);
