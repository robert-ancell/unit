#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new X11 screen containing:
/// - a [root] window.
/// - a [default_colormap].
/// - the [current_input_masks].
/// - the values for a [white_pixel] and [black_pixel].
/// - dimensions of [width_in_pixels]x[height_in_pixels].
/// - physical dimensions of [width_in_millimeters]x[height_in_millimeters].
/// - available [visuals], with [root_visual] specifing the one that [root] is
/// using.
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

/// Returns the window ID for the root window on this screen.
uint32_t ut_x11_screen_get_root(UtObject *object);

/// Returns the visual the root window on this screen.
///
/// !return-type UtX11Visual
UtObject *ut_x11_screen_get_root_visual(UtObject *object);

/// Returns the visuals on this screen.
///
/// !return-type UtObjectList
UtObject *ut_x11_screen_get_visuals(UtObject *object);

/// Returns [true] if [object] is a [UtX11Screen].
bool ut_object_is_x11_screen(UtObject *object);
