#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"
#include "ut-tiff-tag.h"

#pragma once

UtObject *
ut_tiff_image_new(uint32_t width, uint32_t length,
                  UtTiffPhotometricInterpretation photometric_interpretation,
                  UtTiffPlanarConfiguration planar_configuration,
                  uint16_t bits_per_sample, uint16_t samples_per_pixel,
                  UtObject *data);

UtObject *ut_tiff_image_new_from_data(UtObject *data);

uint32_t ut_tiff_image_get_width(UtObject *object);

uint32_t ut_tiff_image_get_length(UtObject *object);

UtTiffPhotometricInterpretation
ut_tiff_image_get_photometric_interpretation(UtObject *object);

UtTiffPlanarConfiguration
ut_tiff_image_get_planar_configuration(UtObject *object);

uint16_t ut_tiff_image_get_bits_per_sample(UtObject *object);

uint16_t ut_tiff_image_get_samples_per_pixel(UtObject *object);

size_t ut_tiff_image_get_row_stride(UtObject *object);

void ut_tiff_image_set_color_map(UtObject *object, UtObject *color_map);

UtObject *ut_tiff_image_get_color_map(UtObject *object);

UtObject *ut_tiff_image_get_data(UtObject *object);

UtObject *ut_tiff_image_to_rgba(UtObject *object);

/// Returns [true] if [object] is a [UtTiffImage].
bool ut_object_is_tiff_image(UtObject *object);
