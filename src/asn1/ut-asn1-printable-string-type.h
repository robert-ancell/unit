#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 PrintableString type.
///
/// !return-ref
/// !return-type UtAsn1PrintableStringType
UtObject *ut_asn1_printable_string_type_new();

/// Returns [true] if [object] is a [UtAsn1PrintableStringType].
bool ut_object_is_asn1_printable_string_type(UtObject *object);
