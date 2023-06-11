#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_gif_encoder_new(uint16_t width, uint16_t height,
                             UtObject *global_color_table, UtObject *images,
                             UtObject *output_stream);

UtObject *ut_gif_encoder_new_single(UtObject *image, UtObject *output_stream);

void ut_gif_encoder_add_comment(UtObject *object, const char *comment);

void ut_gif_encoder_encode(UtObject *object);

bool ut_object_is_gif_encoder(UtObject *object);
