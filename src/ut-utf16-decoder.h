#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtUtf16Decoder] to decode UTF-16 code units from [input].
///
/// !arg-type input UtUint16List
/// !return-type UtUtf16Decoder
/// !return-ref
UtObject *ut_utf16_decoder_new(UtObject *input);

/// Returns [true] if [object] is a [UtUtf16Decoder].
bool ut_object_is_utf16_decoder(UtObject *object);
