#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new GIF renderer from [decoder].
///
/// !arg-type decoder UtGifDecoder
/// !return-ref
/// !return-type UtGifRenderer
UtObject *ut_gif_renderer_new(UtObject *decoder);

/// Returns a rendered image in RGB format.
///
/// !return-ref
/// !return-type UtUint8List
UtObject *ut_gif_renderer_render(UtObject *object);

/// Returns [true] if [object] is a [UtGifRenderer].
bool ut_object_is_gif_renderer(UtObject *object);
