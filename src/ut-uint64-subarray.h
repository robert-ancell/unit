#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new array that is a subset of [parent] starting at [start] and
/// containing [length] elements.
///
/// !arg-type parent UtUint64Array
/// !return-ref
/// !return-type UtUint64Subarray
UtObject *ut_uint64_subarray_new(UtObject *parent, size_t start, size_t length);

/// Returns [true] if [object] is a [UtUint64Subarray].
bool ut_object_is_uint64_subarray(UtObject *object);
