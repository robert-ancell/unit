#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1CharacterStringType] object.
///
/// !return-ref
UtObject *ut_asn1_character_string_type_new();

/// Returns [true] if [object] is a [UtAsn1CharacterStringType].
bool ut_object_is_asn1_character_string_type(UtObject *object);
