#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtZlibDecoder] to encode the data from [input_stream].
///
/// !return-type UtZlibDecoder
UtObject *ut_zlib_decoder_new(UtObject *input_stream);

/// Gets the compression level reported in the data.
/// Only valid once the decompression is complete.
uint8_t ut_zlib_decoder_get_compression_level(UtObject *object);

/// Returns [true] if [object] is a [UtZLibDecoder].
bool ut_object_is_zlib_decoder(UtObject *object);
