#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 GraphicString type.
///
/// !return-ref
/// !return-type UtAsn1GraphicStringType
UtObject *ut_asn1_graphic_string_type_new();

/// Returns [true] if [object] is a [UtAsn1GraphicStringType].
bool ut_object_is_asn1_graphic_string_type(UtObject *object);
