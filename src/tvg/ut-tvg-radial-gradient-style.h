#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new TVG radial gradient style with [color0] at [x0],[y0] going to
/// [color1] at [x1],[y1].
///
/// !arg-type color0 UtColor
/// !arg-type color1 UtColor
/// !return-ref
/// !return-type UtTvgRadialGradientStyle
UtObject *ut_tvg_radial_gradient_style_new(float x0, float y0, float x1,
                                           float y1, UtObject *color0,
                                           UtObject *color1);

/// Returns [true] if [object] is a [UtTvgRadialGradientStyle].
bool ut_object_is_tvg_radial_gradient_style(UtObject *object);
