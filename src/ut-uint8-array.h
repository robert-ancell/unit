#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new array that contains unsigned 8 bit values.
///
/// !return-type UtUint8Array
/// !return-ref
UtObject *ut_uint8_array_new();

/// Creates a new array of [length] zeros.
///
/// !return-type UtUint8Array
/// !return-ref
UtObject *ut_uint8_array_new_sized(size_t length);

/// Creates a new array of [data_length] values from [data].
///
/// !return-type UtUint8Array
/// !return-ref
UtObject *ut_uint8_array_new_from_data(const uint8_t *data, size_t data_length);

/// Creates a new array of [length] values.
///
/// !return-type UtUint8Array
/// !return-ref
UtObject *ut_uint8_array_new_from_elements(size_t length, ...);

/// Creates a new array of [length] values.
///
/// !return-type UtUint8Array
/// !return-ref
UtObject *ut_uint8_array_new_from_va_elements(size_t length, va_list ap);

/// Creates a new array of unsigned 8 bit data in [hex], e.g. "6a54c3".
/// Returns an error if [hex] contains invalid characters or not full 8 bit
/// values. !return-type UtUint8Array UtError !return-ref
UtObject *ut_uint8_array_new_from_hex_string(const char *hex);

/// Returns [true] if [object] is a [UtUint8Array].
bool ut_object_is_uint8_array(UtObject *object);
