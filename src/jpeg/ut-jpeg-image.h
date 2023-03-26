#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef enum {
  UT_JPEG_DENSITY_UNITS_NONE,
  UT_JPEG_DENSITY_UNITS_DOTS_PER_INCH,
  UT_JPEG_DENSITY_UNITS_DOTS_PER_CENTIMETER
} UtJpegDensityUnits;

UtObject *ut_jpeg_image_new(uint16_t width, uint16_t height,
                            UtJpegDensityUnits density_units,
                            uint16_t horizontal_pixel_density,
                            uint16_t vertical_pixel_density,
                            size_t n_components, UtObject *data);

uint16_t ut_jpeg_image_get_width(UtObject *object);

uint16_t ut_jpeg_image_get_height(UtObject *object);

UtJpegDensityUnits ut_jpeg_image_get_density_units(UtObject *object);

uint16_t ut_jpeg_image_get_horizontal_pixel_density(UtObject *object);

uint16_t ut_jpeg_image_get_vertical_pixel_density(UtObject *object);

size_t ut_jpeg_image_get_n_components(UtObject *object);

UtObject *ut_jpeg_image_get_data(UtObject *object);

bool ut_object_is_jpeg_image(UtObject *object);
