#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new X11 expose event.
///
/// !return-ref
/// !return-type UtX11Expose
UtObject *ut_x11_expose_new(uint32_t window, uint16_t x, uint16_t y,
                            uint16_t width, uint16_t height);

uint32_t ut_x11_expose_get_window(UtObject *object);

uint16_t ut_x11_expose_get_x(UtObject *object);

uint16_t ut_x11_expose_get_y(UtObject *object);

uint16_t ut_x11_expose_get_width(UtObject *object);

uint16_t ut_x11_expose_get_height(UtObject *object);

/// Returns [true] if [object] is a [UtX11Expose].
bool ut_object_is_x11_expose(UtObject *object);
