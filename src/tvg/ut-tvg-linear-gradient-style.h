#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new TVG linear gradient style with [color0] at [x0],[y0] going to
/// [color1] at [x1],[y1].
///
/// !arg-type color0 UtColor
/// !arg-type color1 UtColor
/// !return-ref
/// !return-type UtTvgLinearGradientStyle
UtObject *ut_tvg_linear_gradient_style_new(float x0, float y0, float x1,
                                           float y1, UtObject *color0,
                                           UtObject *color1);

/// Returns the first color used in this style.
///
/// !return-type UtColor
UtObject *ut_tvg_flat_colored_style_get_color0(UtObject *object);

/// Returns the second color used in this style.
///
/// !return-type UtColor
UtObject *ut_tvg_flat_colored_style_get_color1(UtObject *object);

/// Returns [true] if [object] is a [UtTvgLinearGradientStyle].
bool ut_object_is_tvg_linear_gradient_style(UtObject *object);
