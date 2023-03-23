#include "ut.h"
#include "ut-jpeg-huffman-decoder.h"

typedef enum {
  DECODER_STATE_DC_COEFFICIENT1,
  DECODER_STATE_DC_COEFFICIENT2,
  DECODER_STATE_AC_COEFFICIENT1,
  DECODER_STATE_AC_COEFFICIENT2,
} DecoderState;

typedef struct {
  UtObject object;

  // Input stream being read.
  UtObject *input_stream;
  UtObject *read_cancel;

  // Callback to notify when complete.
  UtJpegHuffmanDecodeCallback callback;
  void *user_data;
  UtObject *cancel;

} UtJpegHuffmanDecoder;

// Read a the next scan byte from [data] and write it to [value].
static bool read_scan_byte(UtJpegHuffmanDecoder *self, UtObject *data, size_t *offset,
                           uint8_t *value) {
  size_t data_length = ut_list_get_length(data);

  size_t o = *offset;
  if (o >= data_length) {
    return false;
  }

  uint8_t byte1 = ut_uint8_list_get_element(data, o++);

  // Scan data terminates on a marker. If 0xff is in the scan data, 0x00 is
  // after it so it can't be a valid marker. The 0x00 is dropped.
  if (byte1 == 0xff) {
    if (o >= data_length) {
      return false;
    }
    uint8_t byte2 = ut_uint8_list_get_element(data, o++);
    if (byte2 != 0x00) {
      self->state = DECODER_STATE_MARKER;
      return false;
    }
  }

  *offset = o;
  *value = byte1;
  return true;
}

// Read the next bit from [data] and write it to [value].
static bool read_scan_bit(UtJpegHuffmanDecoder *self, UtObject *data, size_t *offset,
                          uint8_t *value) {
  if (self->bit_count == 0) {
    if (!read_scan_byte(self, data, offset, &self->bit_buffer)) {
      return false;
    }
    self->bit_count = 8;
  }

  *value = self->bit_buffer >> 7;
  self->bit_buffer <<= 1;
  self->bit_count--;

  return true;
}

// Read the next Huffman symbol from [data] using [decoder] and write it to
// [symbol].
static bool read_huffman_symbol(UtJpegHuffmanDecoder *self, UtObject *data,
                                size_t *offset, UtObject *decoder,
                                uint16_t *symbol) {
  while (true) {
    uint8_t bit;
    if (!read_scan_bit(self, data, offset, &bit)) {
      return false;
    }
    self->code = self->code << 1 | bit;
    self->code_width++;
    if (ut_huffman_decoder_get_symbol(decoder, self->code, self->code_width,
                                      symbol)) {
      self->code = 0;
      self->code_width = 0;
      return true;
    }
  }
}

// Read an integer of [length] bits from [data].
static bool read_int(UtJpegHuffmanDecoder *self, UtObject *data, size_t *offset,
                     size_t length, uint16_t *value) {
  while (self->code_width < length) {
    uint8_t bit;
    if (!read_scan_bit(self, data, offset, &bit)) {
      return false;
    }
    // FIXME: Use bit_buffer instead of self->code
    self->code = self->code << 1 | bit;
    self->code_width++;
  }

  *value = self->code;
  self->code = 0;
  self->code_width = 0;

  return true;
}

// Add a coefficient [value] to the current data unit. Add [run_length] zeros
// before this coefficient.
static void add_coefficient(UtJpegHuffmanDecoder *self, size_t run_length,
                            int16_t value) {
  JpegComponent *component = self->scan_components[self->scan_component_index];
  const uint8_t *quantization_table_data =
      ut_uint8_list_get_data(component->quantization_table);

  if (self->data_unit_coefficient_index + run_length >
      self->scan_coefficient_end) {
    set_error(self, "Too many coefficients in data unit");
    return;
  }

  // Pad with zeros.
  for (size_t i = 0; i < run_length; i++) {
    uint8_t index = self->data_unit_order[self->data_unit_coefficient_index];
    self->encoded_data_unit[index] = 0;
    self->data_unit_coefficient_index++;
  }

  // Put cofficient into data unit in zig-zag order.
  uint8_t index = self->data_unit_order[self->data_unit_coefficient_index];
  self->encoded_data_unit[index] = value * quantization_table_data[index];

  if (self->data_unit_coefficient_index < self->scan_coefficient_end) {
    self->data_unit_coefficient_index++;
    self->state = DECODER_STATE_AC_COEFFICIENT1;
  } else {
    process_data_unit(self);
  }
}

// Read an amplitude of [length] bits.
static bool read_amplitude(UtJpegHuffmanDecoder *self, UtObject *data, size_t *offset,
                           size_t length, int16_t *amplitude) {
  if (length == 0) {
    *amplitude = 0;
    return true;
  }

  uint16_t value;
  if (!read_int(self, data, offset, length, &value)) {
    return false;
  }

  // Upper half of values are positive, lower half are negative, i.e.
  // 0 bits:  0
  // 1 bit:  -1, 1
  // 2 bits: -3,-2, 2, 3
  // 3 bits: -7,-6,-5,-4, 4, 5, 6, 7
  // ...
  int16_t min_amplitude = 1 << (length - 1);
  if (value >= min_amplitude) {
    *amplitude = value;
  } else {
    *amplitude = value - (min_amplitude * 2) + 1;
  }

  return true;
}

static size_t decode_dc_coefficient1(UtJpegHuffmanDecoder *self, UtObject *data,
                                     bool complete) {
  JpegComponent *component = self->scan_components[self->scan_component_index];

  size_t offset = 0;
  uint16_t symbol;
  if (!read_huffman_symbol(self, data, &offset, component->dc_decoder,
                           &symbol)) {
    return offset;
  }

  self->coefficient_start =
      ut_uint8_list_get_element(component->dc_table, symbol);

  self->state = DECODER_STATE_DC_COEFFICIENT2;

  return offset;
}

static size_t decode_dc_coefficient2(UtJpegHuffmanDecoder *self, UtObject *data,
                                     bool complete) {
  JpegComponent *component = self->scan_components[self->scan_component_index];

  size_t offset = 0;
  int16_t diff = 0;
  size_t coefficient_length = self->coefficient_start;
  if (!read_amplitude(self, data, &offset, coefficient_length, &diff)) {
    return false;
  }

  int16_t dc = component->previous_dc + diff;
  component->previous_dc = dc;
  add_coefficient(self, 0, dc);

  return offset;
}

static size_t decode_ac_coefficient1(UtJpegHuffmanDecoder *self, UtObject *data,
                                     bool complete) {
  JpegComponent *component = self->scan_components[self->scan_component_index];

  size_t offset = 0;
  uint16_t symbol;
  if (!read_huffman_symbol(self, data, &offset, component->ac_decoder,
                           &symbol)) {
    return offset;
  }

  self->coefficient_start =
      ut_uint8_list_get_element(component->ac_table, symbol);

  size_t coefficient_length = self->coefficient_start & 0xf;
  if (coefficient_length == 0) {
    size_t run_length = self->coefficient_start >> 4;

    // Special cases of fill to end of data unit, and fill with 16 zeros
    if (run_length == 0) {
      add_coefficient(
          self, self->scan_coefficient_end - self->data_unit_coefficient_index,
          0);
    } else if (run_length == 15) {
      add_coefficient(self, 15, 0);
    } else {
      // FIXME: undefined
      assert(false);
    }
  } else {
    self->state = DECODER_STATE_AC_COEFFICIENT2;
  }

  return offset;
}

static size_t decode_ac_coefficient2(UtJpegHuffmanDecoder *self, UtObject *data,
                                     bool complete) {
  size_t offset = 0;
  size_t run_length = self->coefficient_start >> 4;
  size_t coefficient_length = self->coefficient_start & 0xf;
  int16_t ac;
  if (!read_amplitude(self, data, &offset, coefficient_length, &ac)) {
    return false;
  }
  add_coefficient(self, run_length, ac);

  return offset;
}

static size_t read_cb(void *user_data, UtObject *data, bool complete) {
  UtJpegHuffmanDecoder *self = user_data;

  if (ut_cancel_is_active(self->cancel)) {
    ut_cancel_activate(self->read_cancel);
    return 0;
  }

  if (ut_object_implements_error(data)) {
    set_error(self, "Failed to read JPEG Huffman data: %s",
              ut_error_get_description(data));
    return 0;
  }

  size_t data_length = ut_list_get_length(data);
  size_t offset = 0;
  while (true) {
    size_t n_used;
    UtObjectRef d = ut_list_get_sublist(data, offset, data_length - offset);
    DecoderState old_state = self->state;
    switch (self->state) {
    case DECODER_STATE_DC_COEFFICIENT1:
      n_used = decode_dc_coefficient1(self, d, complete);
      break;
    case DECODER_STATE_DC_COEFFICIENT2:
      n_used = decode_dc_coefficient2(self, d, complete);
      break;
    case DECODER_STATE_AC_COEFFICIENT1:
      n_used = decode_ac_coefficient1(self, d, complete);
      break;
    case DECODER_STATE_AC_COEFFICIENT2:
      n_used = decode_ac_coefficient2(self, d, complete);
      break;
    default:
      assert(false);
    }

    offset += n_used;
    if (self->state == old_state && n_used == 0) {
      if (complete && self->state != DECODER_STATE_DONE) {
        set_error(self, "Incomplete JPEG");
      }
      return offset;
    }
  }
}

static void ut_jpeg_huffman_decoder_init(UtObject *object) {
  UtJpegHuffmanDecoder *self = (UtJpegHuffmanDecoder *)object;
  self->read_cancel = ut_cancel_new();
}

static void ut_jpeg_huffman_decoder_cleanup(UtObject *object) {
  UtJpegHuffmanDecoder *self = (UtJpegHuffmanDecoder *)object;
  ut_cancel_activate(self->read_cancel);
  ut_object_unref(self->input_stream);
  ut_object_unref(self->read_cancel);
}

static UtObjectInterface object_interface = {.type_name = "UtJpegHuffmanDecoder",
                                             .init = ut_jpeg_huffman_decoder_init,
                                             .cleanup = ut_jpeg_huffman_decoder_cleanup,
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_jpeg_huffman_decoder_new(UtObject *input_stream) {
  UtObject *object = ut_object_new(sizeof(UtJpegHuffmanDecoder), &object_interface);
  UtJpegHuffmanDecoder *self = (UtJpegHuffmanDecoder *)object;
  self->input_stream = ut_object_ref(input_stream);
  return object;
}

void ut_jpeg_huffman_decoder_decode(UtObject *object, UtJpegDecodeCallback callback,
                            void *user_data, UtObject *cancel) {
  assert(ut_object_is_jpeg_huffman_decoder(object));
  UtJpegHuffmanDecoder *self = (UtJpegHuffmanDecoder *)object;

  assert(self->callback == NULL);
  assert(callback != NULL);

  self->callback = callback;
  self->user_data = user_data;
  self->cancel = ut_object_ref(cancel);

  ut_input_stream_read(self->input_stream, read_cb, self, self->read_cancel);
}

bool ut_object_is_jpeg_huffman_decoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
