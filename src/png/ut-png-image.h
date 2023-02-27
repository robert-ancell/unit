#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef enum {
  UT_PNG_COLOUR_TYPE_GREYSCALE,
  UT_PNG_COLOUR_TYPE_TRUECOLOUR,
  UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
  UT_PNG_COLOUR_TYPE_GREYSCALE_WITH_ALPHA,
  UT_PNG_COLOUR_TYPE_TRUECOLOUR_WITH_ALPHA
} UtPngColourType;

UtObject *ut_png_image_new(uint32_t width, uint32_t height, uint8_t bit_depth,
                           UtPngColourType colour_type, UtObject *palette,
                           UtObject *data);

uint32_t ut_png_image_get_width(UtObject *object);

uint32_t ut_png_image_get_height(UtObject *object);

uint8_t ut_png_image_get_bit_depth(UtObject *object);

UtPngColourType ut_png_image_get_colour_type(UtObject *object);

size_t ut_png_image_get_n_channels(UtObject *object);

size_t ut_png_image_get_row_stride(UtObject *object);

UtObject *ut_png_image_get_palette(UtObject *object);

UtObject *ut_png_image_get_data(UtObject *object);

bool ut_object_is_png_image(UtObject *object);
