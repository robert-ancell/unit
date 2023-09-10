#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new object that contains a single signed 32 bit [value].
///
/// !return-type UtInt32
/// !return-ref
UtObject *ut_int32_new(int32_t value);

/// Returns the value of [object].
int32_t ut_int32_get_value(UtObject *object);

/// Returns [true] if [object] is a [UtInt32].
bool ut_object_is_int32(UtObject *object);