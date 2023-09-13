#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new X11 screen.
///
/// !arg-type root_visual UtX11Visual
/// !arg-type visuals UtObjectList
/// !return-ref
/// !return-type UtX11Screen
UtObject *ut_x11_screen_new(uint32_t root, uint32_t default_colormap,
                            uint32_t white_pixel, uint32_t black_pixel,
                            uint32_t current_input_masks,
                            uint16_t width_in_pixels, uint16_t height_in_pixels,
                            uint16_t width_in_millimeters,
                            uint16_t height_in_millimeters,
                            UtObject *root_visual, UtObject *visuals);

uint32_t ut_x11_screen_get_root(UtObject *object);

UtObject *ut_x11_screen_get_root_visual(UtObject *object);

UtObject *ut_x11_screen_get_visuals(UtObject *object);

/// Returns [true] if [object] is a [UtX11Screen].
bool ut_object_is_x11_screen(UtObject *object);
