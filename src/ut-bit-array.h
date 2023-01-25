#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new array that contains bits.
UtObject *ut_bit_array_new();

UtObject *ut_bit_array_new_from_bin_string(const char *bin);

uint8_t *ut_bit_array_get_data(UtObject *object);

bool ut_object_is_bit_array(UtObject *object);
