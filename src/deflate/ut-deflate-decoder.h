#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtDeflateDecoder] to read deflate data from [input_stream].
///
/// !return-type UtDeflateDecoder
/// !return-ref
UtObject *ut_deflate_decoder_new(UtObject *input_stream);

/// Returns [true] if [object] is a [UtDeflateDecoder].
bool ut_object_is_deflate_decoder(UtObject *object);
