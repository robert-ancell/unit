#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new object that contains a single unsigned 64 bit [value].
///
/// !return-type UtUint64
/// !return-ref
UtObject *ut_uint64_new(uint64_t value);

/// Returns the value in this object.
uint64_t ut_uint64_get_value(UtObject *object);

/// Returns [true] if [object] is a [UtUint64].
bool ut_object_is_uint64(UtObject *object);
