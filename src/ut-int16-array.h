#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new array that contains signed 16 bit values.
///
/// !return-ref
/// !return-type UtInt16Array
UtObject *ut_int16_array_new();

/// Creates a new array of [length] zeros.
///
/// !return-ref
/// !return-type UtInt16Array
UtObject *ut_int16_array_new_sized(size_t length);

/// Creates a new array that contains [length] signed 16 bit values.
///
/// !return-ref
/// !return-type UtInt16Array
UtObject *ut_int16_array_new_from_elements(size_t length, ...);

/// Creates a new array that contains [length] signed 16 bit values from [ap].
///
/// !return-ref
/// !return-type UtInt16Array
UtObject *ut_int16_array_new_from_va_elements(size_t length, va_list ap);

/// Returns [true] if [object] is a [UtInt16Array].
bool ut_object_is_int16_array(UtObject *object);
