#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new array that is a subset of [parent] starting at [start] and
/// containing [length] elements.
///
/// !arg-type parent UtInt32Array
/// !return-ref
/// !return-type UtInt32Subarray
UtObject *ut_int32_subarray_new(UtObject *parent, size_t start, size_t length);

/// Returns [true] if [object] is a [UtInt32Subarray].
bool ut_object_is_int32_subarray(UtObject *object);
