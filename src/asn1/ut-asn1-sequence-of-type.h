#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 SEQUENCE OF type containing objects with [type].
///
/// !return-ref
/// !return-type UtAsn1SequenceOfType
UtObject *ut_asn1_sequence_of_type_new(UtObject *type);

UtObject *ut_asn1_sequence_of_type_get_type(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1SequenceOfType].
bool ut_object_is_asn1_sequence_of_type(UtObject *object);
