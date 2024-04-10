#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new TVG draw lines command.
///
/// !arg-type style UtTvgStyle
/// !arg-type points UtFloat32List
/// !return-ref
/// !return-type UtTvgDrawLinesCommand
UtObject *ut_tvg_draw_lines_command_new(UtObject *style, float line_width,
                                        UtObject *points);

/// Returns the style used for the lines.
///
/// !return-type UtTvgStyle
UtObject *ut_tvg_draw_lines_command_get_style(UtObject *object);

/// Returns the line width.
float ut_tvg_draw_lines_command_get_line_width(UtObject *object);

/// Returns the points that make up the lines.
///
/// !return-type UtFloat32List
UtObject *ut_tvg_draw_lines_command_get_points(UtObject *object);

/// Returns [true] if [object] is a [UtTvgDrawLinesCommand].
bool ut_object_is_tvg_draw_lines_command(UtObject *object);
