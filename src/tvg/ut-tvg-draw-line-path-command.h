#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new TVG fill path command.
///
/// !arg-type style UtTvgStyle
/// !arg-type segments UtObjectList
/// !return-ref
/// !return-type UtTvgDrawLinePathCommand
UtObject *ut_tvg_draw_line_path_command_new(UtObject *style, float line_width,
                                            UtObject *segments);

/// Returns the style used for the line.
///
/// !return-type UtTvgStyle
UtObject *ut_tvg_draw_line_path_command_get_style(UtObject *object);

/// Returns the path segments.
///
/// !return-type UtObjectList
UtObject *ut_tvg_draw_line_path_command_get_segments(UtObject *object);

/// Returns [true] if [object] is a [UtTvgDrawLinePathCommand].
bool ut_object_is_tvg_draw_line_path_command(UtObject *object);
