#include <stdbool.h>

#include "ut-object.h"
#include "ut-zlib.h"

#pragma once

/// Creates a new ZLib encoder to encode the data from [input_stream].
///
/// !return-ref
/// !return-type UtZlibEncoder
UtObject *ut_zlib_encoder_new(UtObject *input_stream);

/// Creates a new ZLib encoder to encode the data from [input_stream] using
/// the given [compression_level] and [window_size].
///
/// !return-ref
/// !return-type UtZlibEncoder
UtObject *ut_zlib_encoder_new_full(UtZlibCompressionLevel compression_level,
                                   size_t window_size, UtObject *input_stream);

/// Returns [true] if [object] is a [UtZLibEncoder].
bool ut_object_is_zlib_encoder(UtObject *object);
