#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new UTF-16 encoder to encode Unicode code points from [input].
///
/// !arg-type input UtUint32List
/// !return-type UtUtf16Encoder
/// !return-ref
UtObject *ut_utf16_encoder_new(UtObject *input);

/// Returns [true] if [object] is a [UtUtf16Encoder].
bool ut_object_is_utf16_encoder(UtObject *object);
