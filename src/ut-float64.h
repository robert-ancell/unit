#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_float64_new(double value);

UtObject *ut_float64_new_plus_infinity();

UtObject *ut_float64_new_minus_infinity();

UtObject *ut_float64_new_not_a_number();

double ut_float64_get_value(UtObject *object);

/// Returns [true] if [object] is a [UtFloat64].
bool ut_object_is_float64(UtObject *object);
