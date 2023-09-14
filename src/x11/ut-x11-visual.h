#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new X11 visual.
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

/// Returns [true] if [object] is a [UtX11Visual].
bool ut_object_is_x11_visual(UtObject *object);
