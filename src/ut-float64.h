#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new object that contains a single 64 bit floating point [value].
///
/// !return-type UtFloat64
/// !return-ref
UtObject *ut_float64_new(double value);

/// Creates a new object that contains a the 64 bit floating point value
/// +Infinity.
///
/// !return-type UtFloat64
/// !return-ref
UtObject *ut_float64_new_plus_infinity();

/// Creates a new object that contains a the 64 bit floating point value
/// -Infinity.
///
/// !return-type UtFloat64
/// !return-ref
UtObject *ut_float64_new_minus_infinity();

/// Creates a new object that contains a the 64 bit floating point value
/// NotANumber.
///
/// !return-type UtFloat64
/// !return-ref
UtObject *ut_float64_new_not_a_number();

/// Returns the floating point value.
double ut_float64_get_value(UtObject *object);

/// Returns [true] if [object] is a [UtFloat64].
bool ut_object_is_float64(UtObject *object);
