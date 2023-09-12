#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new PNG encoder to write [image] to [output_stream].
///
/// !return-ref
/// !return-type UtPngEncoder
UtObject *ut_png_encoder_new(UtObject *image, UtObject *output_stream);

/// Start encoding.
void ut_png_encoder_encode(UtObject *object);

/// Returns [true] if [object] is a [UtPngEncoder].
bool ut_object_is_png_encoder(UtObject *object);
