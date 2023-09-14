#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new GZip encoder to encode the data from [input_stream].
///
/// !arg-type input_stream UtInputStream
/// !return-ref
/// !return-type UtGzipEncoder
UtObject *ut_gzip_encoder_new(UtObject *input_stream);

/// Returns [true] if [object] is a [UtGzipEncoder].
bool ut_object_is_gzip_encoder(UtObject *object);
