#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new UTF-8 encoder to encode codepoints from [input].
///
/// !arg-type input UtUint32List
/// !return-type UtUtf8Encoder
/// !return-ref
UtObject *ut_utf8_encoder_new(UtObject *input);

/// Returns [true] if [object] is a [UtUtf8Encoder].
bool ut_object_is_utf8_encoder(UtObject *object);
