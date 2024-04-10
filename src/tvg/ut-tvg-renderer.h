#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new TVG renderer from [image].
///
/// !arg-type image UtTvgImage
/// !return-ref
/// !return-type UtTvgRenderer
UtObject *ut_tvg_renderer_new(UtObject *image);

/// Returns a rendered image in RGB format.
///
/// !return-ref
/// !return-type UtUint8List
UtObject *ut_tvg_renderer_render(UtObject *object);

/// Returns [true] if [object] is a [UtTvgRenderer].
bool ut_object_is_tvg_renderer(UtObject *object);
