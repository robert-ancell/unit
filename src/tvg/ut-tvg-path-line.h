#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new TVG path line ending at [x],[y].
///
/// !return-ref
/// !return-type UtTvgPathLine
UtObject *ut_tvg_path_line_new(float line_width, float x, float y);

/// Returns [true] if [object] is a [UtTvgPathLine].
bool ut_object_is_tvg_path_line(UtObject *object);
