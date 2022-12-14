#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_x11_pixmap_format_new(uint8_t depth, uint8_t bits_per_pixel,
                                   uint8_t scanline_pad);

uint8_t ut_x11_pixmap_format_get_depth(UtObject *object);

uint8_t ut_x11_pixmap_format_get_bits_per_pixel(UtObject *object);

uint8_t ut_x11_pixmap_format_get_scanline_pad(UtObject *object);

bool ut_object_is_x11_pixmap_format(UtObject *object);
