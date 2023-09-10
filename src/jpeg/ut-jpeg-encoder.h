#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_jpeg_encoder_new(UtObject *image, UtObject *output_stream);

void ut_jpeg_encoder_encode(UtObject *object);

/// Returns [true] if [object] is a [UtJpegEncoder].
bool ut_object_is_jpeg_encoder(UtObject *object);
