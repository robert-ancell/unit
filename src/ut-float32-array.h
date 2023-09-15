#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

#include "ut-object.h"

#pragma once

/// Creates a new array that contains 32 bit floating point values.
///
/// !return-ref
/// !return-type UtFloat32Array
UtObject *ut_float32_array_new();

/// Creates a new array of [length] zeros.
///
/// !return-ref
/// !return-type UtFloat32Array
UtObject *ut_float32_array_new_sized(size_t length);

/// Creates a new array that contains [length] 32 bit floating point values.
///
/// !return-ref
/// !return-type UtFloat32Array
UtObject *ut_float32_array_new_from_elements(size_t length, ...);

/// Creates a new array that contains [length] 32 bit floating point values.
///
/// !return-ref
/// !return-type UtFloat32Array
UtObject *ut_float32_array_new_from_va_elements(size_t length, va_list ap);

/// Returns [true] if [object] is a [UtFloat32Array].
bool ut_object_is_float32_array(UtObject *object);
