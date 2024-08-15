#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new JPEG arithmetic decoder.
///
/// !return-ref
/// !return-type UtJpegArithmeticDecoder
UtObject *ut_jpeg_arithmetic_decoder_new();

/// Read a single bit from [data].
/// Returns [true] if a there is sufficient data to read the bit.
bool ut_jpeg_arithmetic_decoder_read_bit(UtObject *object, UtObject *data,
                                         size_t *offset, uint8_t *bit);

/// Returns [true] if [object] is a [UtJpegArithmeticDecoder].
bool ut_object_is_jpeg_arithmetic_decoder(UtObject *object);
