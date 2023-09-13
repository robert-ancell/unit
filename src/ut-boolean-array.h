#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new array that contains boolean values.
///
/// !return-type UtBooleanArray
/// !return-ref
UtObject *ut_boolean_array_new();

/// Creates a new array of [length] false values.
///
/// !return-type UtBooleanArray
/// !return-ref
UtObject *ut_boolean_array_new_sized(size_t length);

/// Returns the memory used for this array.
/// You may modify the contents.
bool *ut_boolean_array_get_data(UtObject *object);

/// Returns [true] if [object] is a [UtBooleaArray].
bool ut_object_is_boolean_array(UtObject *object);
