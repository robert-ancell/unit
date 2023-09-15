#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 [type] [value].
///
/// !arg-type type UtAsn1Type
/// !arg-type value UtObject
/// !return-ref
/// !return-type UtAsn1TypeValue
UtObject *ut_asn1_type_value_new(UtObject *type, UtObject *value);

/// Returns the type.
///
/// !return-type UtAsn1Type
UtObject *ut_asn1_type_value_get_type(UtObject *object);

/// Returns the value.
///
/// !return-type UtAsn1Object
UtObject *ut_asn1_type_value_get_value(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1TypeValue].
bool ut_object_is_asn1_type_value(UtObject *object);
