#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new decoder to read deflate data from [input_stream].
///
/// !arg-type input_stream UtInputStream
/// !return-type UtDeflateDecoder
/// !return-ref
UtObject *ut_deflate_decoder_new(UtObject *input_stream);

/// Returns [true] if [object] is a [UtDeflateDecoder].
bool ut_object_is_deflate_decoder(UtObject *object);
