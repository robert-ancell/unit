#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_bit_list_new();

UtObject *ut_bit_list_new_from_data(UtObject *data, size_t bit_count);

UtObject *ut_bit_list_new_from_bin_string(const char *bin);

size_t ut_bit_list_get_length(UtObject *object);

bool ut_bit_list_get_element(UtObject *object, size_t index);

UtObject *ut_bit_list_get_data(UtObject *object);

void ut_bit_list_append(UtObject *object, bool value);

void ut_bit_list_append_bits(UtObject *object, uint8_t data, size_t bit_count);

char *ut_bit_list_to_bin_string(UtObject *object);

bool ut_object_is_bit_list(UtObject *object);
