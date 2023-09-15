#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new object identifier containing [values].
///
/// !arg-type values UtUint32List
/// !return-type UtObjectIdentifier
/// !return-ref
UtObject *ut_object_identifier_new(UtObject *values);

/// Creates a new object identifier object from [string], e.g. "1.0.8571.1".
///
/// !return-type UtObjectIdentifier
/// !return-ref
UtObject *ut_object_identifier_new_from_string(const char *string);

/// Returns the values that make up this object identifier.
///
/// !return-type values UtUint32List
UtObject *ut_object_identifier_get_values(UtObject *object);

/// Returns this object identifier in string form, e.g. "1.0.8571.1".
char *ut_object_identifier_to_string(UtObject *object);

/// Returns [true] if [object] is a [UtObjectIdentifier].
bool ut_object_is_object_identifier(UtObject *object);
