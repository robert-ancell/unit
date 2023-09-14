#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new object identifier object.
///
/// !return-type UtObjectIdentifier
/// !return-ref
UtObject *ut_object_identifier_new(UtObject *values);

/// Creates a new object identifier object.
///
/// !return-type UtObjectIdentifier
/// !return-ref
UtObject *ut_object_identifier_new_from_string(const char *hex_string);

UtObject *ut_object_identifier_get_values(UtObject *object);

char *ut_object_identifier_to_string(UtObject *object);

/// Returns [true] if [object] is a [UtObjectIdentifier].
bool ut_object_is_object_identifier(UtObject *object);
