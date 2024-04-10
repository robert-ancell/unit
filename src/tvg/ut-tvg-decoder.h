#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef void (*UtTvgDecodeCallback)(UtObject *object);

/// Creates a new TVG decoder to read an image from [input_stream].
///
/// !arg-type input_stream UtInputStream
/// !return-ref
/// !return-type UtTvgDecoder
UtObject *ut_tvg_decoder_new(UtObject *input_stream);

/// Start decoding.
/// When complete [callback] is called.
void ut_tvg_decoder_decode(UtObject *object, UtObject *callback_object,
                           UtTvgDecodeCallback callback);

/// Starts decoding an image that has all data available.
///
/// !return-ref
/// !return-type UtTvgImage UtTvgError
UtObject *ut_tvg_decoder_decode_sync(UtObject *object);

/// Returns the first error that occurred during decoding or [NULL] if no error
/// occurred.
///
/// !return-type UtTvgError NULL
UtObject *ut_tvg_decoder_get_error(UtObject *object);

/// Returns the image that was decoded or [NULL] if decoding was unsuccessful.
///
/// !return-type UtTvgImage
UtObject *ut_tvg_decoder_get_image(UtObject *object);

/// Returns [true] if [object] is a [UtTvgDecoder].
bool ut_object_is_tvg_decoder(UtObject *object);
