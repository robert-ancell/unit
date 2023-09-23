#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new JPEG encoder to write [image] to [output_stream].
///
/// !arg-type image UtJpegImage
/// !arg-type output_stream UtOutputStream
/// !return-ref
/// !return-type UtJpegEncoder
UtObject *ut_jpeg_encoder_new(UtObject *image, UtObject *output_stream);

/// Start encoding.
void ut_jpeg_encoder_encode(UtObject *object);

/// Returns [true] if [object] is a [UtJpegEncoder].
bool ut_object_is_jpeg_encoder(UtObject *object);
