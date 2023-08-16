#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1RelativeOidType] object.
///
/// !return-ref
UtObject *ut_asn1_relative_oid_type_new();

/// Returns [true] if [object] is a [UtAsn1RelativeOidType].
bool ut_object_is_asn1_relative_oid_type(UtObject *object);
