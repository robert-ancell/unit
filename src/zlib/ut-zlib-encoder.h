#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtZlibEncoder] to encode the data from [input_stream].
///
/// !return-type UtZlibEncoder
UtObject *ut_zlib_encoder_new(UtObject *input_stream);

/// Creates a new [UtZlibEncoder] to encode the data from [input_stream] using
/// the given [window_size].
///
/// !return-type UtZlibEncoder
UtObject *ut_zlib_encoder_new_with_window_size(size_t window_size,
                                               UtObject *input_stream);

/// Returns [true] if [object] is a [UtZLibEncoder].
bool ut_object_is_zlib_encoder(UtObject *object);
