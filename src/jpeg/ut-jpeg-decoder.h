#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef void (*UtJpegDecodeCallback)(UtObject *object);

/// Creates a new JPEG decoder to read an image from [input_stream].
///
/// !return-ref
/// !return-type UtJpegDecoder
UtObject *ut_jpeg_decoder_new(UtObject *input_stream);

/// Start decoding.
/// When complete [callback] is called.
void ut_jpeg_decoder_decode(UtObject *object, UtObject *callback_object,
                            UtJpegDecodeCallback callback);

/// Starts decoding an image that has all data available.
///
/// !return-ref
/// !return-type UtJpegImage UtJpegError
UtObject *ut_jpeg_decoder_decode_sync(UtObject *object);

/// Returns the first error that occurred during decoding or [NULL] if no error
/// occurred.
///
/// !return-type UtJpegError NULL
UtObject *ut_jpeg_decoder_get_error(UtObject *object);

/// Returns the image that was decoded or [NULL] if decoding was unsuccessful.
///
/// !return-type UtJpegImage
UtObject *ut_jpeg_decoder_get_image(UtObject *object);

/// Returns [true] if [object] is a [UtJpegDecoder].
bool ut_object_is_jpeg_decoder(UtObject *object);
