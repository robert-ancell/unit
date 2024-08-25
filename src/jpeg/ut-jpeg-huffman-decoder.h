#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new JPEG decoder to read a scan from [input_stream] containing
/// [components]. The scan contains coefficients
/// [coefficient_start]-[coefficient_end].
///
/// !arg-type input_stream UtInputStream
/// !arg-type components UtObjectList
/// !return-ref
/// !return-type UtJpegHuffmanDecoder
UtObject *ut_jpeg_huffman_decoder_new(UtObject *input_stream,
                                      UtObject *components,
                                      size_t coefficient_start,
                                      size_t coefficient_end);

/// Start decoding.
void ut_jpeg_huffman_decoder_decode(UtObject *object);

/// Returns [true] if [object] is a [UtJpegHuffmanDecoder].
bool ut_object_is_jpeg_huffman_decoder(UtObject *object);
