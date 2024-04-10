#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new TVG path segment starting at [x],[y].
///
/// !arg-type commands UtObjectList
/// !return-ref
/// !return-type UtTvgPathSegment
UtObject *ut_tvg_path_segment_new(float x, float y, UtObject *commands);

/// Returns the commands in this path segments.
///
/// !return-type UtObjectList
UtObject *ut_tvg_path_segment_get_commands(UtObject *object);

/// Returns [true] if [object] is a [UtTvgPathSegment].
bool ut_object_is_tvg_path_segment(UtObject *object);
