#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef void (*UtPngDecodeCallback)(UtObject *object);

UtObject *ut_png_decoder_new(UtObject *input_stream);

void ut_png_decoder_decode(UtObject *object, UtObject *callback_object,
                           UtPngDecodeCallback callback);

UtObject *ut_png_decoder_decode_sync(UtObject *object);

/// Returns the first error that occurred during decoding or [NULL] if no error
/// occurred.
///
/// !return-type UtPngError NULL
UtObject *ut_png_decoder_get_error(UtObject *object);

/// Returns the image that was decoded or [NULL] if decoding was unsuccessful.
///
/// !return-type UtPngImage
UtObject *ut_png_decoder_get_image(UtObject *object);

/// Returns [true] if [object] is a [UtPngDecoder].
bool ut_object_is_png_decoder(UtObject *object);
