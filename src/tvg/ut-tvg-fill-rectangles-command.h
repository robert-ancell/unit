#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new TVG fill rectangles command.
///
/// !arg-type style UtTvgStyle
/// !arg-type values UtFloat32List
/// !return-ref
/// !return-type UtTvgFillRectanglesCommand
UtObject *ut_tvg_fill_rectangles_command_new(UtObject *style, UtObject *values);

/// Returns the style used for the rectangles.
///
/// !return-type UtTvgStyle
UtObject *ut_tvg_fill_rectangles_command_get_style(UtObject *object);

/// Returns the values used for the rectangles.
///
/// !return-type UtFloat32List
UtObject *ut_tvg_fill_rectangles_command_get_values(UtObject *object);

/// Returns [true] if [object] is a [UtTvgFillRectanglesCommand].
bool ut_object_is_tvg_fill_rectangles_command(UtObject *object);
