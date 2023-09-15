#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 SET OF type containing objects with [type].
///
/// !arg-type type UtAsn1Type
/// !return-ref
/// !return-type UtAsn1SetOfType
UtObject *ut_asn1_set_of_type_new(UtObject *type);

/// Returns the type this set containts.
///
/// !return-type UtAsn1Type
UtObject *ut_asn1_set_of_type_get_type(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1SetOfType].
bool ut_object_is_asn1_set_of_type(UtObject *object);
