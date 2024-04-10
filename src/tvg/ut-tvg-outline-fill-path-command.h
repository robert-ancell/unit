#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new TVG fill path command.
///
/// !arg-type fill_style UtTvgStyle
/// !arg-type line_style UtTvgStyle
/// !arg-type path UtTvgPath
/// !return-ref
/// !return-type UtTvgOutlineFillpathCommand
UtObject *ut_tvg_outline_fill_path_command_new(UtObject *fill_style,
                                               UtObject *line_style,
                                               float line_width,
                                               UtObject *path);

/// Returns the fill style used for the path.
///
/// !return-type UtTvgStyle
UtObject *ut_tvg_outline_fill_path_command_get_fill_style(UtObject *object);

/// Returns the line style used for the path.
///
/// !return-type UtTvgStyle
UtObject *ut_tvg_outline_fill_path_command_get_line_style(UtObject *object);

/// Returns the path used.
///
/// !return-type UtTvgPath
UtObject *ut_tvg_outline_fill_path_command_get_path(UtObject *object);

/// Returns [true] if [object] is a [UtTvgOutlineFillpathCommand].
bool ut_object_is_tvg_outline_fill_path_command(UtObject *object);
