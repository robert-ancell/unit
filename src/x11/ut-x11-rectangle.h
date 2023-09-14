#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new X11 rectangle.
///
/// !return-ref
/// !return-type UtX11Rectangle
UtObject *ut_x11_rectangle_new(int16_t x, int16_t y, uint16_t width,
                               uint16_t height);

/// Gets the dimensions of this rectangle and writes them into [x], [y], [width]
/// and [height].
void ut_x11_rectangle_get_dimensions(UtObject *object, int16_t *x, int16_t *y,
                                     uint16_t *width, uint16_t *height);

/// Returns [true] if [object] is a [UtX11Rectangle].
bool ut_object_is_x11_rectangle(UtObject *object);
