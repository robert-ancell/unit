#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new array that is a subset of [parent] starting at [start] and
/// containing [length] elements. !arg-type parent UtInt64Array !return-type
/// UtInt64Subarray !return-ref
UtObject *ut_int64_subarray_new(UtObject *parent, size_t start, size_t length);

/// Returns [true] if [object] is a [UtInt64Subarray].
bool ut_object_is_int64_subarray(UtObject *object);
