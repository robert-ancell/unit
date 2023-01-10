#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1TaggedType] object.
///
/// !return-ref
UtObject *ut_asn1_tagged_type_new(unsigned int number, UtObject *type);

unsigned int ut_asn1_tagged_type_get_number(UtObject *object);

UtObject *ut_asn1_tagged_type_get_type(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1TaggedType].
bool ut_object_is_asn1_tagged_type(UtObject *object);
