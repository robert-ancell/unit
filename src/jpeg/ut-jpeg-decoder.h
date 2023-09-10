#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef void (*UtJpegDecodeCallback)(UtObject *object);

UtObject *ut_jpeg_decoder_new(UtObject *input_stream);

void ut_jpeg_decoder_decode(UtObject *object, UtObject *callback_object,
                            UtJpegDecodeCallback callback);

UtObject *ut_jpeg_decoder_decode_sync(UtObject *object);

UtObject *ut_jpeg_decoder_get_error(UtObject *object);

UtObject *ut_jpeg_decoder_get_image(UtObject *object);

/// Returns [true] if [object] is a [UtJpegDecoder].
bool ut_object_is_jpeg_decoder(UtObject *object);
