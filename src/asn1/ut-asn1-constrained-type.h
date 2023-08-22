#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1ConstrainedType] object.
///
/// !return-ref
UtObject *ut_asn1_constrained_type_new(UtObject *type, UtObject *constraint);

/// Creates a new [UtAsn1ConstrainedType] object.
///
/// !return-ref
UtObject *ut_asn1_constrained_type_new_take(UtObject *type,
                                            UtObject *constraint);

UtObject *ut_asn1_constrained_type_get_type(UtObject *object);

UtObject *ut_asn1_constrained_type_get_constraint(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1ConstrainedType].
bool ut_object_is_asn1_constrained_type(UtObject *object);
