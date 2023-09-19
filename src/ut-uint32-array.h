#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new array that contains unsigned 32 bit values.
///
/// !return-ref
/// !return-type UtUint32Array
UtObject *ut_uint32_array_new();

/// Creates a new array of [length] zeros.
///
/// !return-ref
/// !return-type UtUint32Array
UtObject *ut_uint32_array_new_sized(size_t length);

/// Creates a new list that contains [length] unsigned 32 bit values.
///
/// !return-ref
/// !return-type UtUint32Array
UtObject *ut_uint32_array_new_from_elements(size_t length, ...);

/// Creates a new list that contains [length] unsigned 32 bit values from [ap].
///
/// !return-ref
/// !return-type UtUint32Array
UtObject *ut_uint32_array_new_from_va_elements(size_t length, va_list ap);

/// Returns [true] if [object] is a [UtUint32Array].
bool ut_object_is_uint32_array(UtObject *object);
