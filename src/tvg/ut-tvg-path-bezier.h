#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new TVG path cubic bezier line ending at [x],[y].
///
/// !return-ref
/// !return-type UtTvgPathBezier
UtObject *ut_tvg_path_bezier_new(float line_width, float control_x0,
                                 float control_y0, float control_x1,
                                 float control_y1, float x, float y);

/// Creates a new TVG path quadratic bezier line ending at [x],[y].
///
/// !return-ref
/// !return-type UtTvgPathBezier
UtObject *ut_tvg_path_bezier_new_quadratic(float line_width, float control_x,
                                           float control_y, float x, float y);

/// Returns [true] if [object] is a [UtTvgPathBezier].
bool ut_object_is_tvg_path_bezier(UtObject *object);
