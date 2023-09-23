#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef void (*UtPngDecodeCallback)(UtObject *object);

/// Creates a new PNG decoder to read an image from [input_stream].
///
/// !arg-type input_stream UtInputStream
/// !return-ref
/// !return-type UtPngDecoder
UtObject *ut_png_decoder_new(UtObject *input_stream);

/// Start decoding.
/// When complete [callback] is called.
void ut_png_decoder_decode(UtObject *object, UtObject *callback_object,
                           UtPngDecodeCallback callback);

/// Starts decoding an image that has all data available.
///
/// !return-ref
/// !return-type UtPngImage UtPngError
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
