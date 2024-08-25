#include <assert.h>

#include "ut-jpeg-huffman-decoder.h"

#include "ut-jpeg-scan-component.h"
#include "ut.h"

typedef enum {
  DECODER_STATE_COEFFICIENT_MAGNITUDE,
  DECODER_STATE_COEFFICIENT_AMPLITUDE,
  DECODER_STATE_COEFFICIENT_END_OF_BLOCK_COUNT,
  DECODER_STATE_ERROR
} DecoderState;

typedef struct {
  UtObject object;

  // Input stream being read from.
  UtObject *input_stream;

  // Components being decoded.
  UtObject *components;

  // First coefficient to be received in scan.
  size_t coefficient_start;

  // First coefficient to be received in scan.
  size_t coefficient_end;

  // Current state of the decoder.
  DecoderState state;

  // Current component being read.
  size_t component_index;

  // Current data unit being read.
  size_t data_unit_index;

  // Current coefficient being read.
  size_t coefficient_index;

  // Current bits being read.
  uint8_t bit_buffer;
  uint8_t bit_count;

  // Last read Huffman code.
  uint16_t code;
  uint8_t code_width;

  // DC value from previous data unit.
  // FIXME: Can read from coefficients.
  int16_t previous_dc;

  // Magnitude of coefficient.
  uint8_t coefficient_magnitude;

  // Number of zeros before this coefficient.
  size_t run_length;

  // Error that occurred during decoding.
  UtObject *error;
} UtJpegHuffmanDecoder;

__attribute__((format(printf, 2, 3))) static void
set_error(UtJpegHuffmanDecoder *self, const char *format, ...) {
  if (self->state == DECODER_STATE_ERROR) {
    return;
  }

  va_list ap;
  va_start(ap, format);
  ut_cstring_ref description = ut_cstring_new_vprintf(format, ap);
  va_end(ap);
  self->error = ut_jpeg_error_new(description);
  self->state = DECODER_STATE_ERROR;
}

// Add a coefficient [value] to the current data unit. Add [run_length] zeros
// before this coefficient.
static void add_coefficient(UtJpegHuffmanDecoder *self, size_t run_length,
                            int16_t value) {
  UtObject *component =
      ut_object_list_get_element(self->components, self->component_index);

  const uint8_t *quantization_table_data = ut_uint8_list_get_data(
      ut_jpeg_scan_component_get_quantization_table(component));

  if (self->coefficient_index + run_length > self->coefficient_end) {
    set_error(self, "Too many coefficients in data unit");
    return;
  }

  UtObject *coefficients = ut_jpeg_scan_component_get_coefficients(component);
  int16_t *encoded_data_unit = ut_int16_list_get_writable_data(coefficients);

  // Pad with zeros.
  for (size_t i = 0; i < run_length; i++) {
    encoded_data_unit[i] = 0;
    self->coefficient_index++;
  }

  // Put cofficient into data unit in zig-zag order.
  encoded_data_unit[self->coefficient_index] =
      value * quantization_table_data[self->coefficient_index];

  if (self->coefficient_index < self->coefficient_end) {
    self->coefficient_index++;
  } else {
    self->coefficient_index = self->coefficient_start;
    self->data_unit_index++;
    if (self->data_unit_index >= ut_list_get_length(coefficients) / 64) {
      self->component_index++;
      if (self->component_index >= ut_list_get_length(self->components)) {
        // FIXME: Done
      }
    }
  }
}

// Read a the next scan byte from [data] and write it to [value].
static bool read_byte(UtJpegHuffmanDecoder *self, UtObject *data,
                      size_t *offset, uint8_t *value) {
  size_t data_length = ut_list_get_length(data);

  size_t o = *offset;
  if (o >= data_length) {
    return false;
  }

  uint8_t byte1 = ut_uint8_list_get_element(data, o++);

  // Skip byte stuffing.
  if (byte1 == 0xff) {
    if (o >= data_length) {
      return false;
    }
    uint8_t byte2 = ut_uint8_list_get_element(data, o++);
    assert(byte2 == 0x00);
  }

  *offset = o;
  *value = byte1;
  return true;
}

// Read the next bit from [data] and write it to [value].
static bool read_bit(UtJpegHuffmanDecoder *self, UtObject *data, size_t *offset,
                     uint8_t *value) {
  if (self->bit_count == 0) {
    if (!read_byte(self, data, offset, &self->bit_buffer)) {
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
    if (!read_bit(self, data, offset, &bit)) {
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
    if (!read_bit(self, data, offset, &bit)) {
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

static bool decode_coefficient_magnitude(UtJpegHuffmanDecoder *self,
                                         UtObject *data, size_t *offset) {
  UtObject *component =
      ut_object_list_get_element(self->components, self->component_index);

  UtObject *decoder, *table;
  if (self->coefficient_index == 0) {
    decoder = ut_jpeg_scan_component_get_dc_decoder(component);
    table = ut_jpeg_scan_component_get_dc_table(component);
  } else {
    decoder = ut_jpeg_scan_component_get_ac_decoder(component);
    table = ut_jpeg_scan_component_get_ac_table(component);
  }

  uint16_t symbol;
  if (!read_huffman_symbol(self, data, offset, decoder, &symbol)) {
    return false;
  }
  if (symbol >= ut_list_get_length(table)) {
    set_error(self, "Huffman symbol received not in table");
    return false;
  }
  uint8_t value = ut_uint8_list_get_element(table, symbol);

  if (self->coefficient_index == 0) {
    self->coefficient_magnitude = value;
    self->run_length = 0;
  } else {
    self->coefficient_magnitude = value & 0xf;
    self->run_length = value >> 4;
    if (self->coefficient_magnitude == 0 && self->run_length < 15) {
      self->state = DECODER_STATE_COEFFICIENT_END_OF_BLOCK_COUNT;
      return true;
    }
  }

  self->state = DECODER_STATE_COEFFICIENT_AMPLITUDE;

  return true;
}

static bool decode_coefficient_amplitude(UtJpegHuffmanDecoder *self,
                                         UtObject *data, size_t *offset) {
  int16_t amplitude;
  if (self->coefficient_magnitude == 0) {
    amplitude = 0;
  } else {
    uint16_t value;
    if (!read_int(self, data, offset, self->coefficient_magnitude, &value)) {
      return false;
    }

    // Upper half of values are positive, lower half are negative, i.e.
    // 0 bits:  0
    // 1 bit:  -1, 1
    // 2 bits: -3,-2, 2, 3
    // 3 bits: -7,-6,-5,-4, 4, 5, 6, 7
    // ...
    int16_t min_amplitude = 1 << (self->coefficient_magnitude - 1);
    if (value >= min_amplitude) {
      amplitude = value;
    } else {
      amplitude = value - (min_amplitude * 2) + 1;
    }
  }

  size_t run_length;
  int16_t coefficient;
  if (self->coefficient_index == 0) {
    int16_t dc = self->previous_dc + amplitude;
    self->previous_dc = dc;
    run_length = 0;
    coefficient = dc;
  } else {
    run_length = self->run_length;
    coefficient = amplitude;
  }
  add_coefficient(self, run_length, coefficient);

  self->state = DECODER_STATE_COEFFICIENT_MAGNITUDE;

  return true;
}

static bool decode_coefficient_end_of_block_count(UtJpegHuffmanDecoder *self,
                                                  UtObject *data,
                                                  size_t *offset) {
  size_t length = self->run_length;

  size_t count;
  if (length == 0) {
    count = 1;
  } else {
    uint16_t value;
    if (!read_int(self, data, offset, length, &value)) {
      return false;
    }
    count = (1 << length) + value;
  }

  // Fill remaining coefficients on current data unit.
  add_coefficient(self, self->coefficient_end - self->coefficient_index, 0);

  // Following data units are all empty.
  for (size_t i = 1; i < count; i++) {
    add_coefficient(self, self->coefficient_end - self->coefficient_start, 0);
  }

  self->state = DECODER_STATE_COEFFICIENT_MAGNITUDE;

  return true;
}

// Process scan data.
static size_t read_cb(UtObject *object, UtObject *data, bool complete) {
  UtJpegHuffmanDecoder *self = (UtJpegHuffmanDecoder *)object;

  size_t offset = 0;
  bool decoding;
  do {
    switch (self->state) {
    case DECODER_STATE_COEFFICIENT_MAGNITUDE:
      decoding = decode_coefficient_magnitude(self, data, &offset);
      break;
    case DECODER_STATE_COEFFICIENT_AMPLITUDE:
      decoding = decode_coefficient_amplitude(self, data, &offset);
      break;
    case DECODER_STATE_COEFFICIENT_END_OF_BLOCK_COUNT:
      decoding = decode_coefficient_end_of_block_count(self, data, &offset);
      break;
    case DECODER_STATE_ERROR:
      return offset;
    }
  } while (decoding);

  return offset;
}

static void ut_jpeg_huffman_decoder_init(UtObject *object) {}

static void ut_jpeg_huffman_decoder_cleanup(UtObject *object) {
  UtJpegHuffmanDecoder *self = (UtJpegHuffmanDecoder *)object;

  ut_object_unref(self->input_stream);
  ut_object_unref(self->components);
  ut_object_unref(self->error);
}

static UtObjectInterface object_interface = {
    .type_name = "UtJpegHuffmanDecoder",
    .init = ut_jpeg_huffman_decoder_init,
    .cleanup = ut_jpeg_huffman_decoder_cleanup};

UtObject *ut_jpeg_huffman_decoder_new(UtObject *input_stream,
                                      UtObject *components,
                                      size_t coefficient_start,
                                      size_t coefficient_end) {
  UtObject *object =
      ut_object_new(sizeof(UtJpegHuffmanDecoder), &object_interface);
  UtJpegHuffmanDecoder *self = (UtJpegHuffmanDecoder *)object;
  self->input_stream = ut_object_ref(input_stream);
  self->components = ut_object_ref(components);
  self->coefficient_start = coefficient_start;
  self->coefficient_end = coefficient_end;
  return object;
}

void ut_jpeg_huffman_decoder_decode(UtObject *object) {
  assert(ut_object_is_jpeg_huffman_decoder(object));
  UtJpegHuffmanDecoder *self = (UtJpegHuffmanDecoder *)object;
  ut_input_stream_read(self->input_stream, object, read_cb);
}

bool ut_object_is_jpeg_huffman_decoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
