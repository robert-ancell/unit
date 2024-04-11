#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef void (*UtGifDecodeCallback)(UtObject *object);

/// Creates a new GIF decoder to read an image from [input_stream].
///
/// !arg-type input_stream UtInputStream
/// !return-ref
/// !return-type UtGifDecoder
UtObject *ut_gif_decoder_new(UtObject *input_stream);

/// Start decoding.
/// When complete [callback] is called.
void ut_gif_decoder_decode(UtObject *object, UtObject *callback_object,
                           UtGifDecodeCallback callback);

/// Starts decoding an image that has all data available.
///
/// !return-ref
/// !return-type UtObjectList UtGifError
UtObject *ut_gif_decoder_decode_sync(UtObject *object);

/// Returns the first error that occurred during decoding or [NULL] if no error
/// occurred.
///
/// !return-type UtGifError NULL
UtObject *ut_gif_decoder_get_error(UtObject *object);

/// Returns the width of the image in pixels.
uint16_t ut_gif_decoder_get_width(UtObject *object);

/// Returns the height of the image in pixels.
uint16_t ut_gif_decoder_get_height(UtObject *object);

/// Returns the comments in the image.
///
/// !return-type UtObjectList
UtObject *ut_gif_decoder_get_comments(UtObject *object);

/// Returns the images that make up this GIF image.
///
/// !return-type UtObjectList
UtObject *ut_gif_decoder_get_images(UtObject *object);

/// Returns the number of times to loop the animation.
size_t ut_gif_decoder_get_loop_count(UtObject *object);

/// Returns [true] if [object] is a [UtGifDecoder].
bool ut_object_is_gif_decoder(UtObject *object);
