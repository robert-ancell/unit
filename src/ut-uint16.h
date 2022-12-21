#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtUint16] object that contains [value].
/// !return-ref
UtObject *ut_uint16_new(uint16_t value);

/// Gets the value of [object].
uint16_t ut_uint16_get_value(UtObject *object);

/// Returns [true] if [object] is a [UtUint16].
bool ut_object_is_uint16(UtObject *object);
