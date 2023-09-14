#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new boolean object that contains a boolean [value].
///
/// !return-ref
/// !return-type UtBoolean
UtObject *ut_boolean_new(bool value);

/// Gets the value of this object.
bool ut_boolean_get_value(UtObject *object);

/// Returns [true] if [object] is a [UtBoolean].
bool ut_object_is_boolean(UtObject *object);
