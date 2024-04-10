#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new TVG flat colored style with [color].
///
/// !arg-type color UtColor
/// !return-ref
/// !return-type UtTvgFlatColoredStyle
UtObject *ut_tvg_flat_colored_style_new(UtObject *color);

/// Returns the color used in this style.
///
/// !return-type UtColor
UtObject *ut_tvg_flat_colored_style_get_color(UtObject *object);

/// Returns [true] if [object] is a [UtTvgFlatColoredStyle].
bool ut_object_is_tvg_flat_colored_style(UtObject *object);
