#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new encoder to compress [input_stream].
///
/// !arg-type input_stream UtInputStream
/// !return-type UtDeflateEncoder
/// !return-ref
UtObject *ut_deflate_encoder_new(UtObject *input_stream);

/// Creates a new encoder to compress [input_stream].
/// The [window_size] is the maximum size of the dictionary to use.
///
/// !arg-type input_stream UtInputStream
/// !return-type UtDeflateEncoder
/// !return-ref
UtObject *ut_deflate_encoder_new_with_window_size(size_t window_size,
                                                  UtObject *input_stream);

/// Returns the window size used by this encoder.
size_t ut_deflate_encoder_get_window_size(UtObject *object);

/// Returns [true] if [object] is a [UtDeflateEncoder].
bool ut_object_is_deflate_encoder(UtObject *object);
