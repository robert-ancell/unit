#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef enum {
  UT_JPEG_DENSITY_UNITS_NONE,
  UT_JPEG_DENSITY_UNITS_DOTS_PER_INCH,
  UT_JPEG_DENSITY_UNITS_DOTS_PER_CENTIMETER
} UtJpegDensityUnits;

/// Creates a new JPEG image with dimensions [width] and [height].
/// [data] contains 8 bits image samples that match [n_components].
/// Pixel density is set in [horizontal_pixel_density], [vertical_pixel_density]
/// and [density_units].
///
/// !arg-type data UtUint8List
/// !return-ref
/// !return-type UtJpegImage
UtObject *ut_jpeg_image_new(uint16_t width, uint16_t height,
                            UtJpegDensityUnits density_units,
                            uint16_t horizontal_pixel_density,
                            uint16_t vertical_pixel_density,
                            size_t n_components, UtObject *data);

/// Returns the width of the image in pixels.
uint16_t ut_jpeg_image_get_width(UtObject *object);

/// Returns the height of the image in pixels.
uint16_t ut_jpeg_image_get_height(UtObject *object);

/// Returns the units used for density information.
UtJpegDensityUnits ut_jpeg_image_get_density_units(UtObject *object);

/// Returns the horizontal pixel density in the units set in
/// [ut_jpeg_image_get_density_units].
uint16_t ut_jpeg_image_get_horizontal_pixel_density(UtObject *object);

/// Returns the vertical pixel density in the units set in
/// [ut_jpeg_image_get_density_units].
uint16_t ut_jpeg_image_get_vertical_pixel_density(UtObject *object);

/// Returns the number of components per pixel.
size_t ut_jpeg_image_get_n_components(UtObject *object);

/// Returns the image packed data.
///
/// !return-type UtUint8List
UtObject *ut_jpeg_image_get_data(UtObject *object);

/// Returns [true] if [object] is a [UtJpegImage].
bool ut_object_is_jpeg_image(UtObject *object);
