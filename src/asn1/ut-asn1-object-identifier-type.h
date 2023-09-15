#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1ObjectIdentifierType] object.
///
/// !return-ref
/// !return-type
UtObject *ut_asn1_object_identifier_type_new();

/// Returns [true] if [object] is a [UtAsn1ObjectIdentifierType].
bool ut_object_is_asn1_object_identifier_type(UtObject *object);
