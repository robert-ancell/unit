#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 constrained type with [type] and [constraint].
///
/// !arg-type type UtAsn1Type
/// !arg-type constraint UtObject
/// !return-ref
/// !return-type UtAsn1ConstrainedType
UtObject *ut_asn1_constrained_type_new(UtObject *type, UtObject *constraint);

/// Creates a new ASN.1 constrained type with [type] and [constraint].
///
/// !arg-type type UtAsn1Type
/// !arg-type constraint UtObject
/// !return-ref
/// !return-type UtAsn1ConstrainedType
UtObject *ut_asn1_constrained_type_new_take(UtObject *type,
                                            UtObject *constraint);

/// Returns the type being constrained.
///
/// !return-type UtAsn1Type
UtObject *ut_asn1_constrained_type_get_type(UtObject *object);

/// Returns the constraint applied to this type.
///
/// !return-type UtObject
UtObject *ut_asn1_constrained_type_get_constraint(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1ConstrainedType].
bool ut_object_is_asn1_constrained_type(UtObject *object);
