#include <stdbool.h>

#include "ut-object.h"
#include "ut-zlib.h"

#pragma once

/// Creates a new [UtZlibDecoder] to decode the data from [input_stream].
///
/// !arg-type input_stream UtInputStream
/// !return-ref
/// !return-type UtZlibDecoder
UtObject *ut_zlib_decoder_new(UtObject *input_stream);

/// Gets the compression level reported in the data.
/// Only valid once the decompression is complete.
UtZlibCompressionLevel ut_zlib_decoder_get_compression_level(UtObject *object);

/// Returns [true] if [object] is a [UtZlibDecoder].
bool ut_object_is_zlib_decoder(UtObject *object);
