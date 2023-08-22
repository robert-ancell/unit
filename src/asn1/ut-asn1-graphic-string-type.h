#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1GraphicStringType] object.
///
/// !return-ref
UtObject *ut_asn1_graphic_string_type_new();

/// Returns [true] if [object] is a [UtAsn1GraphicStringType].
bool ut_object_is_asn1_graphic_string_type(UtObject *object);
