#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtZlibEncoder] to encode the data from [input_stream].
///
/// !return-type UtZlibDecoder
UtObject *ut_zlib_decoder_new(UtObject *input_stream);

/// Returns [true] if [object] is a [UtZLibDecoder].
bool ut_object_is_zlib_decoder(UtObject *object);
