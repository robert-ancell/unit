#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

void ut_huffman_code_generate(UtObject *symbol_weights, uint16_t *codes,
                              size_t *code_widths);

bool ut_huffman_code_generate_canonical(UtObject *code_widths, uint16_t *codes);
