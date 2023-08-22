#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1GeneralStringType] object.
///
/// !return-ref
UtObject *ut_asn1_general_string_type_new();

/// Returns [true] if [object] is a [UtAsn1GeneralStringType].
bool ut_object_is_asn1_general_string_type(UtObject *object);
