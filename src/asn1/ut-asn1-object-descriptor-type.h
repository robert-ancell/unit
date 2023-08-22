#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1ObjectDescriptorType] object.
///
/// !return-ref
UtObject *ut_asn1_object_descriptor_type_new();

/// Returns [true] if [object] is a [UtAsn1ObjectDescriptorType].
bool ut_object_is_asn1_object_descriptor_type(UtObject *object);
