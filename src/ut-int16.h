#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new object that contains a single signed 16 bit [value].
///
/// !return-type UtInt16
/// !return-ref
UtObject *ut_int16_new(int16_t value);

/// Returns the value of [object].
int16_t ut_int16_get_value(UtObject *object);

/// Returns [true] if [object] is a [UtInt16].
bool ut_object_is_int16(UtObject *object);