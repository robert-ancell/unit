#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_arithmetic_decoder_new(uint8_t value);

bool ut_arithmetic_decoder_get_symbol(UtObject *object, uint16_t code,
                                      size_t code_width, uint16_t *symbol);

bool ut_object_is_arithmetic_decoder(UtObject *object);
