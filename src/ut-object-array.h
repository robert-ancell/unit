#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new array that contains objects.
///
/// !return-ref
/// !return-type UtObjectArray
UtObject *ut_object_array_new();

/// Creates a new array from the NULL terminated list of objects [item0].
///
/// !arg-type item0 UtObject
/// !return-ref
/// !return-type UtObjectArray
UtObject *ut_object_array_new_from_elements(UtObject *item0, ...);

/// Creates a new array from the NULL terminated list of objects [item0].
/// Each object passes in a reference.
///
/// !arg-type item0 UtObject
/// !return-ref
/// !return-type UtObjectArray
UtObject *ut_object_array_new_from_elements_take(UtObject *item0, ...);

/// Returns [true] if [object] is a [UtObjectArray].
bool ut_object_is_object_array(UtObject *object);
