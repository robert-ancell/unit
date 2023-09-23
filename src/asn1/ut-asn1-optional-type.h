#include <stdbool.h>

#include "ut-asn1-tag.h"
#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 optional [type].
///
/// !arg-type type UtAsn1Type
/// !return-ref
/// !return-type UtAsn1OptionalType
UtObject *ut_asn1_optional_type_new(UtObject *type);

/// Creates a new ASN.1 optional [type].
///
/// !arg-type type UtAsn1Type
/// !return-ref
/// !return-type UtAsn1OptionalType
UtObject *ut_asn1_optional_type_new_take(UtObject *type);

/// Returns the type that has the optional value.
///
/// !return-type UtAsn1Type
UtObject *ut_asn1_optional_type_get_type(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1OptionalType].
bool ut_object_is_asn1_optional_type(UtObject *object);
