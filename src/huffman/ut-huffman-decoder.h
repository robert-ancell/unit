#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_huffman_decoder_new(UtObject *symbol_weights);

UtObject *ut_huffman_decoder_new_canonical(UtObject *code_widths);

bool ut_huffman_decoder_get_symbol(UtObject *object, uint16_t code,
                                   size_t code_width, uint16_t *symbol);

/// Returns [true] if [object] is a [UtHuffmanDecoder].
bool ut_object_is_huffman_decoder(UtObject *object);
