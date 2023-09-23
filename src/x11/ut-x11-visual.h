#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new X11 visual with [id] and [class].
/// Pixels are of [depth] bits.
/// [colormap_entries] defines the number of colors used in this visual.
/// An RGB value is defined by [bits_per_rgb_value] and [red_mask], [green_mask]
/// and [blue_mask].
///
/// !return-ref
/// !return-type UtX11Visual
UtObject *ut_x11_visual_new(uint32_t id, uint8_t depth, uint8_t class,
                            uint8_t bits_per_rgb_value,
                            uint16_t colormap_entries, uint32_t red_mask,
                            uint32_t green_mask, uint32_t blue_mask);

/// Returns the ID for this visual.
uint32_t ut_x11_visual_get_id(UtObject *object);

/// Returns the bit depth of this visual.
uint8_t ut_x11_visual_get_depth(UtObject *object);

/// Returns the number of bits fro an RGB value in this visual.
uint8_t ut_x11_visual_get_bits_per_rgb_value(UtObject *object);

/// Returns the bit mask for the red bits of an RGB value in this visual.
uint32_t ut_x11_visual_get_red_mask(UtObject *object);

/// Returns the bit mask for the green bits of an RGB value in this visual.
uint32_t ut_x11_visual_get_green_mask(UtObject *object);

/// Returns the bit mask for the blue bits of an RGB value in this visual.
uint32_t ut_x11_visual_get_blue_mask(UtObject *object);

/// Returns [true] if [object] is a [UtX11Visual].
bool ut_object_is_x11_visual(UtObject *object);
