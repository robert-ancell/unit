#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_png_encoder_new(UtObject *image, UtObject *output_stream);

void ut_png_encoder_encode(UtObject *object);

bool ut_object_is_png_encoder(UtObject *object);
