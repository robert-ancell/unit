#include <assert.h>

#include "ut-jpeg-scan-component.h"

#include "ut.h"

typedef struct {
  UtObject object;

  // Huffman decoder for DC coefficients.
  UtObject *dc_decoder;

  // Table of values for DC Huffman codes.
  UtObject *dc_table;

  // Huffman decoder for AC coefficients.
  UtObject *ac_decoder;

  // Table of values for AC Huffman codes.
  UtObject *ac_table;

  // Coefficient quantization values.
  UtObject *quantization_table;

  // Decoded coefficients.
  UtObject *coefficients;
} UtJpegScanComponent;

static void ut_jpeg_scan_component_init(UtObject *object) {}

static void ut_jpeg_scan_component_cleanup(UtObject *object) {
  UtJpegScanComponent *self = (UtJpegScanComponent *)object;

  ut_object_unref(self->dc_decoder);
  ut_object_unref(self->dc_table);
  ut_object_unref(self->ac_decoder);
  ut_object_unref(self->ac_table);
  ut_object_unref(self->quantization_table);
  ut_object_unref(self->coefficients);
}

static UtObjectInterface object_interface = {
    .type_name = "UtJpegScanComponent",
    .init = ut_jpeg_scan_component_init,
    .cleanup = ut_jpeg_scan_component_cleanup};

UtObject *ut_jpeg_scan_component_new(UtObject *dc_decoder, UtObject *dc_table,
                                     UtObject *ac_decoder, UtObject *ac_table,
                                     UtObject *quantization_table,
                                     UtObject *coefficients) {
  UtObject *object =
      ut_object_new(sizeof(UtJpegScanComponent), &object_interface);
  UtJpegScanComponent *self = (UtJpegScanComponent *)object;
  self->dc_decoder = ut_object_ref(dc_decoder);
  self->dc_table = ut_object_ref(dc_table);
  self->ac_decoder = ut_object_ref(ac_decoder);
  self->ac_table = ut_object_ref(ac_table);
  self->quantization_table = ut_object_ref(quantization_table);
  self->coefficients = ut_object_ref(coefficients);
  return object;
}

UtObject *ut_jpeg_scan_component_get_dc_decoder(UtObject *object) {
  assert(ut_object_is_jpeg_scan_component(object));
  UtJpegScanComponent *self = (UtJpegScanComponent *)object;
  return self->dc_decoder;
}

UtObject *ut_jpeg_scan_component_get_dc_table(UtObject *object) {
  assert(ut_object_is_jpeg_scan_component(object));
  UtJpegScanComponent *self = (UtJpegScanComponent *)object;
  return self->dc_table;
}

UtObject *ut_jpeg_scan_component_get_ac_decoder(UtObject *object) {
  assert(ut_object_is_jpeg_scan_component(object));
  UtJpegScanComponent *self = (UtJpegScanComponent *)object;
  return self->ac_decoder;
}

UtObject *ut_jpeg_scan_component_get_ac_table(UtObject *object) {
  assert(ut_object_is_jpeg_scan_component(object));
  UtJpegScanComponent *self = (UtJpegScanComponent *)object;
  return self->ac_table;
}

UtObject *ut_jpeg_scan_component_get_quantization_table(UtObject *object) {
  assert(ut_object_is_jpeg_scan_component(object));
  UtJpegScanComponent *self = (UtJpegScanComponent *)object;
  return self->quantization_table;
}

UtObject *ut_jpeg_scan_component_get_coefficients(UtObject *object) {
  assert(ut_object_is_jpeg_scan_component(object));
  UtJpegScanComponent *self = (UtJpegScanComponent *)object;
  return self->coefficients;
}

bool ut_object_is_jpeg_scan_component(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
