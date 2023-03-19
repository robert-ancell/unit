#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_arithmetic_encoder_new(uint8_t value);

void ut_arithmetic_encoder_get_code(UtObject *object, uint16_t symbol,
                                    uint16_t *code, size_t *code_width);

bool ut_object_is_arithmetic_encoder(UtObject *object);
