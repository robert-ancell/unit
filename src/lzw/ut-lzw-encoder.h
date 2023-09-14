#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new LZW encoder with [n_symbols] to encode data from
/// [input_stream] with a dictionary of size [max_dictionary_length]. Codes are
/// written least significant bit first.
///
/// !arg-type input_stream UtInputStream.
/// !return-ref
/// !return-type UtLzwEncoder
UtObject *ut_lzw_encoder_new_lsb(size_t n_symbols, size_t max_dictionary_length,
                                 UtObject *input_stream);

/// Creates a new LZW encoder with [n_symbols] to encode data from
/// [input_stream] with a dictionary of size [max_dictionary_length]. Codes are
/// written most significant bit first.
///
/// !arg-type input_stream UtInputStream.
/// !return-ref
/// !return-type UtLzwEncoder
UtObject *ut_lzw_encoder_new_msb(size_t n_symbols, size_t max_dictionary_length,
                                 UtObject *input_stream);

/// Returns [true] if [object] is a [UtLzwEncoder].
bool ut_object_is_lzw_encoder(UtObject *object);
