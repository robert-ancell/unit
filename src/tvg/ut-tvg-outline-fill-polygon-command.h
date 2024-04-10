#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new TVG fill polygon command.
///
/// !arg-type fill_style UtTvgStyle
/// !arg-type line_style UtTvgStyle
/// !arg-type points UtFloat32List
/// !return-ref
/// !return-type UtTvgOutlineFillPolygonCommand
UtObject *ut_tvg_outline_fill_polygon_command_new(UtObject *fill_style,
                                                  UtObject *line_style,
                                                  float line_width,
                                                  UtObject *points);

/// Returns the fill style used for the polygon.
///
/// !return-type UtTvgStyle
UtObject *ut_tvg_outline_fill_polygon_command_get_fill_style(UtObject *object);

/// Returns the line style used for the polygon.
///
/// !return-type UtTvgStyle
UtObject *ut_tvg_outline_fill_polygon_command_get_line_style(UtObject *object);

/// Returns the points used for the polygon.
///
/// !return-type UtFloat32List
UtObject *ut_tvg_outline_fill_polygon_command_get_points(UtObject *object);

/// Returns [true] if [object] is a [UtTvgOutlineFillPolygonCommand].
bool ut_object_is_tvg_outline_fill_polygon_command(UtObject *object);
