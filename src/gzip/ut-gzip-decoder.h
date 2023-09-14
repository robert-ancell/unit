#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new GZip decoder to decode the data from [input_stream].
///
/// !arg-type input_stream UtInputStream
/// !return-ref
/// !return-type UtGzipDecoder
UtObject *ut_gzip_decoder_new(UtObject *input_stream);

/// Returns [true] if [object] is a [UtGzipDecoder].
bool ut_object_is_gzip_decoder(UtObject *object);
