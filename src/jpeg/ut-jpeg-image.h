#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_jpeg_image_new(uint16_t width, uint16_t height,
                            size_t n_components, UtObject *data);

uint16_t ut_jpeg_image_get_width(UtObject *object);

uint16_t ut_jpeg_image_get_height(UtObject *object);

size_t ut_jpeg_image_get_n_components(UtObject *object);

UtObject *ut_jpeg_image_get_data(UtObject *object);

bool ut_object_is_jpeg_image(UtObject *object);
