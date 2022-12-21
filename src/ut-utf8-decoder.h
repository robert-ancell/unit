#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtUtf8Decoder] to decode UTF-8 data from [input].
///
/// !arg-type input UtUint8List
/// !return-type UtUtf8Decoder
/// !return-ref
UtObject *ut_utf8_decoder_new(UtObject *input);

/// Returns [true] if [object] is a [UtUtf8Decoder].
bool ut_object_is_utf8_decoder(UtObject *object);
