#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"
#include "ut-tiff-tag.h"

#pragma once

/// Creates a new TIFF image with dimensions [width] and [length].
///
/// !arg-type data UtUint8List
/// !return-ref
/// !return-type UtTiffImage
UtObject *
ut_tiff_image_new(uint32_t width, uint32_t length,
                  UtTiffPhotometricInterpretation photometric_interpretation,
                  UtTiffPlanarConfiguration planar_configuration,
                  uint16_t bits_per_sample, uint16_t samples_per_pixel,
                  UtObject *data);

/// Creates a new TIFF image from [data].
///
/// !arg-type data UtUint8List
/// !return-ref
/// !return-type UtTiffImage UtTiffError
UtObject *ut_tiff_image_new_from_data(UtObject *data);

/// Returns the width of the image in pixels.
uint32_t ut_tiff_image_get_width(UtObject *object);

/// Returns the length of the image in pixels.
uint32_t ut_tiff_image_get_length(UtObject *object);

/// Returns the method used to convert the samples to color values.
UtTiffPhotometricInterpretation
ut_tiff_image_get_photometric_interpretation(UtObject *object);

/// Returns how the components of each pixel are stored.
UtTiffPlanarConfiguration
ut_tiff_image_get_planar_configuration(UtObject *object);

/// Returns the number of bits per sample.
uint16_t ut_tiff_image_get_bits_per_sample(UtObject *object);

/// Returns the number of samples per pixel.
uint16_t ut_tiff_image_get_samples_per_pixel(UtObject *object);

/// Returns the row stride of the image data.
size_t ut_tiff_image_get_row_stride(UtObject *object);

/// Sets the [color_map] for this image.
///
/// !arg-type color_map UtUint16List
void ut_tiff_image_set_color_map(UtObject *object, UtObject *color_map);

/// Returns the color map for this image.
///
/// !return-type UtUint16List
UtObject *ut_tiff_image_get_color_map(UtObject *object);

/// Returns the image data.
///
/// !return-type UtUint8List
UtObject *ut_tiff_image_get_data(UtObject *object);

/// Returns the image data converted to RGBA form.
///
/// !return-ref
/// !return-type UtUint8List
UtObject *ut_tiff_image_to_rgba(UtObject *object);

/// Returns [true] if [object] is a [UtTiffImage].
bool ut_object_is_tiff_image(UtObject *object);
