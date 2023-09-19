#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new object that contains a single signed 64 bit [value].
///
/// !return-type UtInt64
/// !return-ref
UtObject *ut_int64_new(int64_t value);

/// Returns the value in this object.
int64_t ut_int64_get_value(UtObject *object);

/// Returns [true] if [object] is a [UtInt64].
bool ut_object_is_int64(UtObject *object);
