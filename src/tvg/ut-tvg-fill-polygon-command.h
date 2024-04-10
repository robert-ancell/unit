#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new TVG fill polygon command.
///
/// !arg-type style UtTvgStyle
/// !arg-type points UtFloat32List
/// !return-ref
/// !return-type UtTvgFillPolygonCommand
UtObject *ut_tvg_fill_polygon_command_new(UtObject *style, UtObject *points);

/// Returns the style used for the polygon.
///
/// !return-type UtTvgStyle
UtObject *ut_tvg_fill_polygon_command_get_style(UtObject *object);

/// Returns the points used for the polygon.
///
/// !return-type UtFloat32List
UtObject *ut_tvg_fill_polygon_command_get_points(UtObject *object);

/// Returns [true] if [object] is a [UtTvgFillPolygonCommand].
bool ut_object_is_tvg_fill_polygon_command(UtObject *object);
