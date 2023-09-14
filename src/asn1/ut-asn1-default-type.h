#include <stdbool.h>

#include "ut-asn1-tag.h"
#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 [type] with [default_value].
///
/// !arg-type default_value UtObject
/// !return-ref
/// !return-type UtAsn1DefaultType
UtObject *ut_asn1_default_type_new(UtObject *type, UtObject *default_value);

/// Creates a new ASN.1 [type] with [default_value].
///
/// !arg-type default_value UtObject
/// !return-ref
/// !return-type UtAsn1DefaultType
UtObject *ut_asn1_default_type_new_take(UtObject *type,
                                        UtObject *default_value);

/// Returns the type that has the default value.
///
/// !return-type UtAsn1Type
UtObject *ut_asn1_default_type_get_type(UtObject *object);

/// Returns the default value.
///
/// !return-type UtObject
UtObject *ut_asn1_default_type_get_default_value(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1DefaultType].
bool ut_object_is_asn1_default_type(UtObject *object);
