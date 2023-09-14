#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new LZW decoder with [n_symbols] to decode data from
/// [input_stream] with a dictionary of size [max_dictionary_length]. Codes are
/// read least significant bit first.
///
/// !arg-type input_stream UtInputStream.
/// !return-ref
/// !return-type UtLzwDecoder
UtObject *ut_lzw_decoder_new_lsb(size_t n_symbols, size_t max_dictionary_length,
                                 UtObject *input_stream);

/// Creates a new LZW decoder with [n_symbols] to decode data from
/// [input_stream] with a dictionary of size [max_dictionary_length]. Codes are
/// read most significant bit first.
///
/// !arg-type input_stream UtInputStream.
/// !return-ref
/// !return-type UtLzwDecoder
UtObject *ut_lzw_decoder_new_msb(size_t n_symbols, size_t max_dictionary_length,
                                 UtObject *input_stream);

/// Returns [true] if [object] is a [UtLzwDecoder].
bool ut_object_is_lzw_decoder(UtObject *object);
