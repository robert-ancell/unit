#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef enum {
  UT_PNG_COLOR_TYPE_GREYSCALE,
  UT_PNG_COLOR_TYPE_TRUECOLOR,
  UT_PNG_COLOR_TYPE_INDEXED_COLOR,
  UT_PNG_COLOR_TYPE_GREYSCALE_WITH_ALPHA,
  UT_PNG_COLOR_TYPE_TRUECOLOR_WITH_ALPHA
} UtPngColorType;

UtObject *ut_png_image_new(uint32_t width, uint32_t height, uint8_t bit_depth,
                           UtPngColorType color_type, UtObject *data);

uint32_t ut_png_image_get_width(UtObject *object);

uint32_t ut_png_image_get_height(UtObject *object);

uint8_t ut_png_image_get_bit_depth(UtObject *object);

UtPngColorType ut_png_image_get_color_type(UtObject *object);

size_t ut_png_image_get_n_channels(UtObject *object);

size_t ut_png_image_get_row_stride(UtObject *object);

void ut_png_image_set_palette(UtObject *object, UtObject *palette);

UtObject *ut_png_image_get_palette(UtObject *object);

void ut_png_image_set_background_color(UtObject *object,
                                       UtObject *background_color);

UtObject *ut_png_image_get_background_color(UtObject *object);

void ut_png_image_set_text(UtObject *object, const char *keyword,
                           const char *text);

void ut_png_image_set_international_text(UtObject *object, const char *keyword,
                                         const char *language,
                                         const char *translated_keyword,
                                         const char *text);

const char *ut_png_image_get_text(UtObject *object, const char *keyword);

const char *
ut_png_image_get_international_text(UtObject *object, const char *keyword,
                                    const char *language,
                                    const char **translated_keyword);

UtObject *ut_png_image_get_data(UtObject *object);

UtObject *ut_png_image_to_rgba(UtObject *object);

bool ut_object_is_png_image(UtObject *object);
