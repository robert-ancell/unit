#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new array that contains unsigned 8 bit values.
UtObject *ut_uint8_array_new();

UtObject *ut_uint8_array_new_from_data(const uint8_t *data, size_t data_length);

UtObject *ut_uint8_array_new_from_elements(size_t length, ...);

UtObject *ut_uint8_array_new_from_va_elements(size_t length, va_list ap);

UtObject *ut_uint8_array_new_from_hex_string(const char *hex);

uint8_t *ut_uint8_array_get_data(UtObject *object);

bool ut_object_is_uint8_array(UtObject *object);
