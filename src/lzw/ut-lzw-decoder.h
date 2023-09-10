#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_lzw_decoder_new_lsb(size_t n_symbols, size_t max_dictionary_length,
                                 UtObject *input_stream);

UtObject *ut_lzw_decoder_new_msb(size_t n_symbols, size_t max_dictionary_length,
                                 UtObject *input_stream);

/// Returns [true] if [object] is a [UtLzwDecoder].
bool ut_object_is_lzw_decoder(UtObject *object);
