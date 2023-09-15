#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new array that contains a constant value of [data_length] unsigned
/// 8 bit values from [data].
///
/// !return-ref
/// !return-type UtConstantUint8Array
UtObject *ut_constant_uint8_array_new(const uint8_t *data, size_t data_length);

/// Returns [true] if [object] is a [UtConstantUint8Array].
bool ut_object_is_constant_uint8_array(UtObject *object);
