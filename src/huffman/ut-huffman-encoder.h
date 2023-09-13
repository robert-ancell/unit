#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_huffman_encoder_new(UtObject *symbol_weights);

UtObject *ut_huffman_encoder_new_canonical(UtObject *code_widths);

void ut_huffman_encoder_get_code(UtObject *object, uint16_t symbol,
                                 uint16_t *code, size_t *code_width);

/// Returns [true] if [object] is a [UtHuffmanEncoder].
bool ut_object_is_huffman_encoder(UtObject *object);
