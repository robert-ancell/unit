#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1SequenceOfType] object.
///
/// !return-ref
UtObject *ut_asn1_sequence_of_type_new(UtObject *type);

UtObject *ut_asn1_sequence_of_type_get_type(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1SequenceOfType].
bool ut_object_is_asn1_sequence_of_type(UtObject *object);
