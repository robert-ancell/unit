#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new Huffman decoder with [symbol_weights].
///
/// !arg-type symbol_weights UtFloat64List
/// !return-ref
/// !return-type UtHuffmanDecoder
UtObject *ut_huffman_decoder_new(UtObject *symbol_weights);

/// Creates a new Huffman decoder with canonical encoding using [code_widths].
///
/// !arg-type code_width UtUint8List
/// !return-ref
/// !return-type UtHuffmanDecoder
UtObject *ut_huffman_decoder_new_canonical(UtObject *code_widths);

/// Returns true if [code] and [code_width] containins data that matches
/// [symbol].
bool ut_huffman_decoder_get_symbol(UtObject *object, uint16_t code,
                                   size_t code_width, uint16_t *symbol);

/// Returns [true] if [object] is a [UtHuffmanDecoder].
bool ut_object_is_huffman_decoder(UtObject *object);
