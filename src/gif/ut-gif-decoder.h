#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef void (*UtGifDecodeCallback)(UtObject *object);

UtObject *ut_gif_decoder_new(UtObject *input_stream);

void ut_gif_decoder_decode(UtObject *object, UtObject *callback_object,
                           UtGifDecodeCallback callback);

UtObject *ut_gif_decoder_decode_sync(UtObject *object);

UtObject *ut_gif_decoder_get_error(UtObject *object);

uint16_t ut_gif_decoder_get_width(UtObject *object);

uint16_t ut_gif_decoder_get_height(UtObject *object);

UtObject *ut_gif_decoder_get_comments(UtObject *object);

UtObject *ut_gif_decoder_get_images(UtObject *object);

size_t ut_gif_decoder_get_loop_count(UtObject *object);

/// Returns [true] if [object] is a [UtGifDecoder].
bool ut_object_is_gif_decoder(UtObject *object);
