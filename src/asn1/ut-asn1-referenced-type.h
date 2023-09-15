#include <stdbool.h>

#include "ut-asn1-tag.h"
#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 referenced type with [type].
///
/// !arg-type type UtAsn1Type
/// !return-ref
/// !return-type UtAsn1ReferencedType
UtObject *ut_asn1_referenced_type_new(UtObject *type);

/// Returns the type that is being referenced.
///
/// !return-type UtAsn1Type
UtObject *ut_asn1_referenced_type_get_type(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1ReferencedType].
bool ut_object_is_asn1_referenced_type(UtObject *object);
