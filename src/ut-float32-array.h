#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

#include "ut-object.h"

#pragma once

UtObject *ut_float32_array_new();

UtObject *ut_float32_array_new_sized(size_t length);

UtObject *ut_float32_array_new_from_elements(size_t length, ...);

UtObject *ut_float32_array_new_from_va_elements(size_t length, va_list ap);

float *ut_float32_array_get_data(UtObject *object);

/// Returns [true] if [object] is a [UtFloat32Array].
bool ut_object_is_float32_array(UtObject *object);
