#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new X11 pixmap format with [depth] and [bits_per_pixel].
/// Each line is padded to [scanline_pad] bits (8, 16, or 32).
///
/// !return-ref
/// !return-type UtX11PimapFormat
UtObject *ut_x11_pixmap_format_new(uint8_t depth, uint8_t bits_per_pixel,
                                   uint8_t scanline_pad);

/// Returns the bit depth of this format.
uint8_t ut_x11_pixmap_format_get_depth(UtObject *object);

/// Returns the number of bits per pixel in this format (1, 4, 8, 16, 24 or 32).
uint8_t ut_x11_pixmap_format_get_bits_per_pixel(UtObject *object);

/// Returns the number of bits scanlines are padded to in this format (8, 16 or
/// 32).
uint8_t ut_x11_pixmap_format_get_scanline_pad(UtObject *object);

/// Returns [true] if [object] is a [UtX11PixmapFormat].
bool ut_object_is_x11_pixmap_format(UtObject *object);
