#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new array that contains unsigned 16 bit values.
/// !return-type UtUint16Array
/// !return-ref
UtObject *ut_uint16_array_new();

/// Creates a new array of [length] zeros.
/// !return-type UtUint16Array
/// !return-ref
UtObject *ut_uint16_array_new_sized(size_t length);

UtObject *ut_uint16_array_new_from_elements(size_t length, ...);

UtObject *ut_uint16_array_new_from_va_elements(size_t length, va_list ap);

/// Creates a new array of containing data in [hex], e.g. "6a54c3d2".
/// Returns an error if [hex] contains invalid characters or not full 16 bit
/// values. !return-type UtUint8Array UtError !return-ref
UtObject *ut_uint16_array_new_from_hex_string(const char *hex);

/// Returns [true] if [object] is a [UtUint16Array].
bool ut_object_is_uint16_array(UtObject *object);
