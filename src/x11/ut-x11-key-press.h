#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new X11 key press event.
///
/// !return-ref
/// !return-type UtX11KeyPress
UtObject *ut_x11_key_press_new(uint32_t window, uint8_t keycode, int16_t x,
                               int16_t y);

uint32_t ut_x11_key_press_get_window(UtObject *object);

uint8_t ut_x11_key_press_get_keycode(UtObject *object);

int16_t ut_x11_key_press_get_x(UtObject *object);

int16_t ut_x11_key_press_get_y(UtObject *object);

/// Returns [true] if [object] is a [UtX11KeyPress].
bool ut_object_is_x11_key_press(UtObject *object);
