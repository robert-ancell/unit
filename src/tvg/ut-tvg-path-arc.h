#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new TVG path arc ending at [x],[y].
///
/// !return-ref
/// !return-type UtTvgPathArc
UtObject *ut_tvg_path_arc_new(float line_width, float radius_x, float radius_y,
                              float rotation, float x, float y);

/// Creates a new TVG path circular arc ending at [x],[y].
///
/// !return-ref
/// !return-type UtTvgPathArc
UtObject *ut_tvg_path_arc_new_circle(float line_width, float radius, float x,
                                     float y);

/// Returns [true] if [object] is a [UtTvgPathArc].
bool ut_object_is_tvg_path_arc(UtObject *object);
