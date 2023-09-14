#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new Huffman encoder with [symbol_weights].
///
/// !arg-type symbol_weights UtFloat64List
/// !return-ref
/// !return-type UtHuffmanEncoder
UtObject *ut_huffman_encoder_new(UtObject *symbol_weights);

/// Creates a new Huffman encoder with canonical encoding using [code_widths].
///
/// !arg-type code_width UtUint8List
/// !return-ref
/// !return-type UtHuffmanEncoder
UtObject *ut_huffman_encoder_new_canonical(UtObject *code_widths);

/// Gets the code for [symbols] returning the value in [code] and [code_width].
void ut_huffman_encoder_get_code(UtObject *object, uint16_t symbol,
                                 uint16_t *code, size_t *code_width);

/// Returns [true] if [object] is a [UtHuffmanEncoder].
bool ut_object_is_huffman_encoder(UtObject *object);
