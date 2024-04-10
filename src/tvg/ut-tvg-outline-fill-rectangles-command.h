#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new TVG outline fill rectangles command.
///
/// !arg-type fill_style UtTvgStyle
/// !arg-type line_style UtTvgStyle
/// !arg-type values UtFloat32List
/// !return-ref
/// !return-type UtTvgOutlineFillRectanglesCommand
UtObject *ut_tvg_outline_fill_rectangles_command_new(UtObject *fill_style,
                                                     UtObject *line_style,
                                                     float line_width,
                                                     UtObject *values);

/// Returns the fill style used for the rectangles.
///
/// !return-type UtTvgStyle
UtObject *
ut_tvg_outline_fill_rectangles_command_get_fill_style(UtObject *object);

/// Returns the line style used for the rectangles.
///
/// !return-type UtTvgStyle
UtObject *
ut_tvg_outline_fill_rectangles_command_get_line_style(UtObject *object);

/// Returns the values used for the rectangles.
///
/// !return-type UtFloat32List
UtObject *ut_tvg_outline_fill_rectangles_command_get_values(UtObject *object);

/// Returns [true] if [object] is a [UtTvgOutlineFillRectanglesCommand].
bool ut_object_is_tvg_outline_fill_rectangles_command(UtObject *object);
