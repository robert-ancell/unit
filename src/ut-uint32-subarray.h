#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new array that is a subset of [parent] starting at [start] and
/// containing [length] elements.
///
/// !arg-type parent UtUint32Array
/// !return-ref
/// !return-type UtUint32Subarray
UtObject *ut_uint32_subarray_new(UtObject *parent, size_t start, size_t length);

/// Returns [true] if [object] is a [UtUint32Subarray].
bool ut_object_is_uint32_subarray(UtObject *object);
