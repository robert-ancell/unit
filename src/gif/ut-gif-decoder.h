#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef void (*UtGifDecodeCallback)(void *user_data);

UtObject *ut_gif_decoder_new(UtObject *input_stream);

void ut_gif_decoder_decode(UtObject *object, UtGifDecodeCallback callback,
                           void *user_data, UtObject *cancel);

UtObject *ut_gif_decoder_decode_sync(UtObject *object);

UtObject *ut_gif_decoder_get_error(UtObject *object);

uint16_t ut_gif_decoder_get_width(UtObject *object);

uint16_t ut_gif_decoder_get_height(UtObject *object);

UtObject *ut_gif_decoder_get_comments(UtObject *object);

UtObject *ut_gif_decoder_get_images(UtObject *object);

size_t ut_gif_decoder_get_loop_count(UtObject *object);

bool ut_object_is_gif_decoder(UtObject *object);
