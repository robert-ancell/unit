#include <stdbool.h>
#include <stddef.h>

#include "ut-object.h"

#pragma once

/// Creates a new RGBA buffer with dimensions [width]x[height] containing 8 bit
/// samples.
///
/// !return-ref
/// !return-type UtRgba8888Buffer
UtObject *ut_rgba8888_buffer_new(size_t width, size_t height);

/// Sets the pixel at [x],[y] to the color [red][green][blue][alpha].
void ut_rgba8888_buffer_set_pixel(UtObject *object, size_t x, size_t y,
                                  uint8_t red, uint8_t green, uint8_t blue,
                                  uint8_t alpha);

/// Gets the pixel at [x],[y] and writes the colors to
/// [red][green][blue][alpha]. If [x],[y] is outside the image, zeros will be
/// returned.
void ut_rgba8888_buffer_get_pixel(UtObject *object, size_t x, size_t y,
                                  uint8_t *red, uint8_t *green, uint8_t *blue,
                                  uint8_t *alpha);

/// Returns [true] if [object] is a [UtRgba8888Buffer].
bool ut_object_is_rgba8888_buffer(UtObject *object);
