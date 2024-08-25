#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new object for a scan component.
///
/// !arg-type input_stream UtInputStream
/// !arg-type dc_decoder UtHuffmanDecoder
/// !arg-type dc_table UtUint8List
/// !arg-type ac_decoder UtHuffmanDecoder
/// !arg-type ac_table UtUint8List
/// !arg-type quantization_table UtUint8List
/// !arg-type coefficients UtInt16List
/// !return-ref
/// !return-type UtJpegScanComponent
UtObject *ut_jpeg_scan_component_new(UtObject *dc_decoder, UtObject *dc_table,
                                     UtObject *ac_decoder, UtObject *ac_table,
                                     UtObject *quantization_table,
                                     UtObject *coefficients);

UtObject *ut_jpeg_scan_component_get_dc_decoder(UtObject *object);

UtObject *ut_jpeg_scan_component_get_dc_table(UtObject *object);

UtObject *ut_jpeg_scan_component_get_ac_decoder(UtObject *object);

UtObject *ut_jpeg_scan_component_get_ac_table(UtObject *object);

UtObject *ut_jpeg_scan_component_get_quantization_table(UtObject *object);

UtObject *ut_jpeg_scan_component_get_coefficients(UtObject *object);

/// Returns [true] if [object] is a [UtJpegScanComponent].
bool ut_object_is_jpeg_scan_component(UtObject *object);
