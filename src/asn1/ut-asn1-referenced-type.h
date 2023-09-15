#include <stdbool.h>

#include "ut-asn1-tag.h"
#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1ReferencedType] object.
///
/// !return-ref
/// !return-type
UtObject *ut_asn1_referenced_type_new(UtObject *type);

UtObject *ut_asn1_referenced_type_get_type(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1ReferencedType].
bool ut_object_is_asn1_referenced_type(UtObject *object);
