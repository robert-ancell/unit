#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new OpenType decoder to read a font from [data].
///
/// !arg-type data UtUint8List
/// !return-ref
/// !return-type UtOpenTypeDecoder
UtObject *ut_open_type_decoder_new(UtObject *data);

/// Start decoding.
void ut_open_type_decoder_decode(UtObject *object);

/// Returns the font that was decoded or [NULL] if decoding was unsuccessful.
///
/// !return-type UtOpenTypeFont
UtObject *ut_open_type_decoder_get_font(UtObject *object);

/// Returns the first error that occurred during decoding or [NULL] if no error
/// occurred.
///
/// !return-type UtOpenTypeError NULL
UtObject *ut_open_type_decoder_get_error(UtObject *object);

/// Returns [true] if [object] is a [UtOpenTypeDecoder].
bool ut_object_is_open_type_decoder(UtObject *object);
