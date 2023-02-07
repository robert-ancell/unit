#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtGzipEncoder] to encode the data from [input_stream].
///
/// !return-type UtGzipEncoder
UtObject *ut_gzip_encoder_new(UtObject *input_stream);

/// Returns [true] if [object] is a [UtgzipEncoder].
bool ut_object_is_gzip_encoder(UtObject *object);
