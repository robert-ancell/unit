#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef void (*UtX11AuthFileDecodeCallback)(UtObject *object);

/// Creates a new decoder to read X11 authorization records from [input_stream].
///
/// !arg-type input_stream UtInputStream
/// !return-type UtX11AuthFileDecoder
/// !return-ref
UtObject *ut_x11_auth_file_decoder_new(UtObject *input_stream);

/// Start decoding.
/// When complete [callback] is called.
void ut_x11_auth_file_decoder_decode(UtObject *object,
                                     UtObject *callback_object,
                                     UtX11AuthFileDecodeCallback callback);

/// Returns the records decoded.
///
/// !return-type UtObjectList
UtObject *ut_x11_auth_file_decoder_get_records(UtObject *object);

/// Returns [true] if [object] is a [UtX11AuthFileDecoder].
bool ut_object_is_x11_auth_file_decoder(UtObject *object);
