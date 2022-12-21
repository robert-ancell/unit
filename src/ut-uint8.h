#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtUint8] object that contains [value].
///
/// !return-ref
UtObject *ut_uint8_new(uint8_t value);

/// Gets the value of [object].
uint8_t ut_uint8_get_value(UtObject *object);

/// Returns [true] if [object] is a [UtUint8].
bool ut_object_is_uint8(UtObject *object);
