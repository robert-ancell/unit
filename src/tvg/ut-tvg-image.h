#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new TVG image with dimensions [width] and [height] and [commands].
///
/// !arg-type commands UtObjectList
/// !return-ref
/// !return-type UtTvgImage
UtObject *ut_tvg_image_new(uint32_t width, uint32_t height, UtObject *commands);

/// Returns the width of the image in display units.
uint32_t ut_tvg_image_get_width(UtObject *object);

/// Returns the height of the image in display units.
uint32_t ut_tvg_image_get_height(UtObject *object);

/// Returns the commands used in the image.
///
/// !return-type UtObjectList
UtObject *ut_tvg_image_get_commands(UtObject *object);

/// Returns [true] if [object] is a [UtTvgImage].
bool ut_object_is_tvg_image(UtObject *object);
