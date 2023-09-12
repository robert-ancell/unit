#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_float32_new(float value);

UtObject *ut_float32_new_plus_infinity();

UtObject *ut_float32_new_minus_infinity();

UtObject *ut_float32_new_not_a_number();

float ut_float32_get_value(UtObject *object);

/// Returns [true] if [object] is a [UtFloat32].
bool ut_object_is_float32(UtObject *object);
