#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new object that contains a single 32 bit floating point [value].
///
/// !return-type UtFloat32
/// !return-ref
UtObject *ut_float32_new(float value);

/// Creates a new object that contains a the 32 bit floating point value
/// +Infinity.
///
/// !return-type UtFloat32
/// !return-ref
UtObject *ut_float32_new_plus_infinity();

/// Creates a new object that contains a the 32 bit floating point value
/// -Infinity.
///
/// !return-type UtFloat32
/// !return-ref
UtObject *ut_float32_new_minus_infinity();

/// Creates a new object that contains a the 32 bit floating point value
/// NotANumber.
///
/// !return-type UtFloat32
/// !return-ref
UtObject *ut_float32_new_not_a_number();

/// Returns the floating point value.
float ut_float32_get_value(UtObject *object);

/// Returns [true] if [object] is a [UtFloat32].
bool ut_object_is_float32(UtObject *object);
