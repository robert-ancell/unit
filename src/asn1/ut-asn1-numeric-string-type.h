#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 NumericString type.
///
/// !return-ref
/// !return-type UtAsn1NumericStringType
UtObject *ut_asn1_numeric_string_type_new();

/// Returns [true] if [object] is a [UtAsn1NumericStringType].
bool ut_object_is_asn1_numeric_string_type(UtObject *object);
