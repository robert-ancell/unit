#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1TypeValue] object.
///
/// !return-ref
UtObject *ut_asn1_type_value_new(UtObject *type, UtObject *value);

UtObject *ut_asn1_type_value_get_type(UtObject *object);

UtObject *ut_asn1_type_value_get_value(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1TypeValue].
bool ut_object_is_asn1_type_value(UtObject *object);
