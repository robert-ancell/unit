#include <assert.h>
#include <math.h>

#include "ut-jpeg.h"
#include "ut.h"

typedef struct {
  UtObject object;

  // Image being encoded.
  UtObject *image;

  // Output stream being written to.
  UtObject *output_stream;

  // Tables for coefficient quantization values.
  UtObject *quantization_tables[4];

  // Huffman encoders for DC coefficients.
  UtObject *dc_symbols[2]; // FIXME: Can just be a fixed 256 array
  UtObject *dc_encoders[2];

  // Huffman encoders for AC coefficients.
  UtObject *ac_symbols[2];
  UtObject *ac_encoders[2];

  // Order that data unit values are written.
  uint8_t data_unit_order[64];

  // Precalculated cosines for DCT.
  float dct_alpha[8];
  float dct_cos[64];

  // Current bits being written.
  uint32_t bit_buffer;
  size_t bit_buffer_length;
} UtJpegEncoder;

static void build_tables(UtJpegEncoder *self) {
  // Standard Luminance quantization table and Huffman encoders.
  self->quantization_tables[0] = ut_uint8_list_new_from_elements(
      64, 16, 11, 10, 16, 24, 40, 51, 61, 12, 12, 14, 19, 26, 58, 60, 55, 14,
      13, 16, 24, 40, 57, 69, 56, 14, 17, 22, 29, 51, 87, 80, 62, 18, 22, 37,
      56, 68, 109, 103, 77, 24, 35, 55, 64, 81, 104, 113, 92, 49, 64, 78, 87,
      103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99);
  self->dc_symbols[0] =
      ut_uint8_list_new_from_elements(12, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11);
  UtObjectRef luminance_dc_code_widths =
      ut_uint8_list_new_from_elements(12, 2, 3, 3, 3, 3, 3, 4, 5, 6, 7, 8, 9);
  self->dc_encoders[0] =
      ut_huffman_encoder_new_canonical(luminance_dc_code_widths);
  self->ac_symbols[0] = ut_uint8_list_new_from_elements(
      162, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
      0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x21, 0x22,
      0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x31, 0x32, 0x33, 0x34,
      0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46,
      0x47, 0x48, 0x49, 0x4a, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
      0x59, 0x5a, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a,
      0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x81, 0x82,
      0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x91, 0x92, 0x93, 0x94,
      0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6,
      0xa7, 0xa8, 0xa9, 0xaa, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8,
      0xb9, 0xba, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca,
      0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
      0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf0, 0xf1, 0xf2, 0xf3,
      0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa);
  UtObjectRef luminance_ac_code_widths = ut_uint8_list_new_from_elements(
      162, 4, 2, 2, 3, 4, 5, 7, 8, 10, 16, 16, 4, 5, 7, 9, 11, 16, 16, 16, 16,
      16, 5, 8, 10, 12, 16, 16, 16, 16, 16, 16, 6, 9, 12, 16, 16, 16, 16, 16,
      16, 16, 6, 10, 16, 16, 16, 16, 16, 16, 16, 16, 7, 11, 16, 16, 16, 16, 16,
      16, 16, 16, 7, 12, 16, 16, 16, 16, 16, 16, 16, 16, 8, 12, 16, 16, 16, 16,
      16, 16, 16, 16, 9, 15, 16, 16, 16, 16, 16, 16, 16, 16, 9, 16, 16, 16, 16,
      16, 16, 16, 16, 16, 9, 16, 16, 16, 16, 16, 16, 16, 16, 16, 10, 16, 16, 16,
      16, 16, 16, 16, 16, 16, 10, 16, 16, 16, 16, 16, 16, 16, 16, 16, 11, 16,
      16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
      11, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16);
  self->ac_encoders[0] =
      ut_huffman_encoder_new_canonical(luminance_ac_code_widths);

  // Standard Chrominance quantization table and Huffman encoders.
  if (ut_jpeg_image_get_n_components(self->image) > 1) {
    self->quantization_tables[1] = ut_uint8_list_new_from_elements(
        64, 17, 18, 24, 47, 99, 99, 99, 99, 18, 21, 26, 66, 99, 99, 99, 99, 24,
        26, 56, 99, 99, 99, 99, 99, 47, 66, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99);
    self->dc_symbols[1] = ut_uint8_list_new_from_elements(12, 0, 1, 2, 3, 4, 5,
                                                          6, 7, 8, 9, 10, 11);
    UtObjectRef chrominance_dc_code_widths = ut_uint8_list_new_from_elements(
        12, 2, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11);
    self->dc_encoders[1] =
        ut_huffman_encoder_new_canonical(chrominance_dc_code_widths);
    self->ac_symbols[1] = ut_uint8_list_new_from_elements(
        162, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x21, 0x22,
        0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x31, 0x32, 0x33, 0x34,
        0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46,
        0x47, 0x48, 0x49, 0x4a, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
        0x59, 0x5a, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a,
        0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x81, 0x82,
        0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x91, 0x92, 0x93, 0x94,
        0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6,
        0xa7, 0xa8, 0xa9, 0xaa, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8,
        0xb9, 0xba, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca,
        0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
        0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf0, 0xf1, 0xf2, 0xf3,
        0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa);
    UtObjectRef chrominance_ac_code_widths = ut_uint8_list_new_from_elements(
        162, 2, 2, 3, 4, 5, 5, 6, 7, 9, 10, 12, 4, 6, 8, 9, 11, 12, 16, 16, 16,
        16, 5, 8, 10, 12, 15, 16, 16, 16, 16, 16, 5, 8, 10, 12, 16, 16, 16, 16,
        16, 16, 6, 9, 16, 16, 16, 16, 16, 16, 16, 16, 6, 10, 16, 16, 16, 16, 16,
        16, 16, 16, 7, 11, 16, 16, 16, 16, 16, 16, 16, 16, 7, 11, 16, 16, 16,
        16, 16, 16, 16, 16, 8, 16, 16, 16, 16, 16, 16, 16, 16, 16, 9, 16, 16,
        16, 16, 16, 16, 16, 16, 16, 9, 16, 16, 16, 16, 16, 16, 16, 16, 16, 9,
        16, 16, 16, 16, 16, 16, 16, 16, 16, 9, 16, 16, 16, 16, 16, 16, 16, 16,
        16, 11, 16, 16, 16, 16, 16, 16, 16, 16, 16, 14, 16, 16, 16, 16, 16, 16,
        16, 16, 16, 10, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16);
    self->ac_encoders[1] =
        ut_huffman_encoder_new_canonical(chrominance_ac_code_widths);
  }
}

// Create the data unit at [data_unit_x], [data_unit_y] containing the given
// image [component]. and write into [data_unit].
static void create_data_unit(UtJpegEncoder *self, size_t data_unit_x,
                             size_t data_unit_y, size_t component,
                             int16_t *data_unit) {
  uint16_t image_width = ut_jpeg_image_get_width(self->image);
  uint16_t image_height = ut_jpeg_image_get_height(self->image);
  size_t n_components = ut_jpeg_image_get_n_components(self->image);
  const uint8_t *image_data =
      ut_uint8_list_get_data(ut_jpeg_image_get_data(self->image));

  for (size_t y = 0; y < 8; y++) {
    for (size_t x = 0; x < 8; x++) {
      size_t ix = data_unit_x * 8 + x;
      if (ix >= image_width) {
        ix = image_width - 1;
      }
      size_t iy = data_unit_y * 8 + y;
      if (iy >= image_height) {
        iy = image_height - 1;
      }
      int16_t value = image_data[(iy * image_width * n_components) +
                                 (ix * n_components) + component];
      data_unit[(y * 8) + x] = (value - 128);
    }
  }
}

// Write the integer [value] of [length] bits to [buffer].
static void write_int(UtJpegEncoder *self, UtObject *buffer,
                      size_t value_length, uint16_t value) {
  self->bit_buffer = self->bit_buffer << value_length | value;
  self->bit_buffer_length += value_length;
  while (self->bit_buffer_length >= 8) {
    size_t shift = self->bit_buffer_length - 8;
    uint8_t byte = self->bit_buffer >> shift;
    ut_uint8_list_append(buffer, byte);

    // Stuff zeros in to stop a marker appearing in the data.
    if (byte == 0xff) {
      ut_uint8_list_append(buffer, 0x00);
    }

    self->bit_buffer ^= byte << shift;
    self->bit_buffer_length -= 8;
  }
}

// Write remaining partial byte to the buffer.
static void end_bits(UtJpegEncoder *self, UtObject *buffer) {
  if (self->bit_buffer_length == 0) {
    return;
  }

  // Pad remaining space with 1 bits.
  size_t length = 8 - self->bit_buffer_length;
  write_int(self, buffer, length, 0xff >> self->bit_buffer_length);
}

// Write [amplitude] using [length] bits to [buffer].
static void write_amplitude(UtJpegEncoder *self, UtObject *buffer,
                            size_t length, int16_t amplitude) {
  // Upper half of values are positive, lower half are negative, i.e.
  // 0 bits:  0
  // 1 bit:  -1, 1
  // 2 bits: -3,-2, 2, 3
  // 3 bits: -7,-6,-5,-4, 4, 5, 6, 7
  // ...
  int16_t min_amplitude = 1 << (length - 1);
  uint16_t value;
  if (amplitude > 0) {
    value = amplitude;
  } else {
    value = amplitude + (min_amplitude * 2) - 1;
  }
  write_int(self, buffer, length, value);
}

// Get the number of bits required to encode [amplitude].
static size_t get_amplitude_length(int16_t amplitude) {
  if (amplitude < 0) {
    amplitude = -amplitude;
  }

  size_t length = 0;
  while (amplitude != 0) {
    amplitude /= 2;
    length++;
  }

  return length;
}

static uint16_t get_symbol(UtObject *symbols, uint8_t value) {
  for (size_t i = 0; i < ut_list_get_length(symbols); i++) {
    if (ut_uint8_list_get_element(symbols, i) == value) {
      return i;
    }
  }

  assert(false);
}

static void write_huffman_code(UtJpegEncoder *self, UtObject *buffer,
                               UtObject *symbols, UtObject *encoder,
                               uint8_t value) {
  uint16_t symbol = get_symbol(symbols, value);
  uint16_t code;
  size_t code_width;
  ut_huffman_encoder_get_code(encoder, symbol, &code, &code_width);
  write_int(self, buffer, code_width, code);
}

static void write_dc_coefficient(UtJpegEncoder *self, UtObject *buffer,
                                 UtObject *symbols, UtObject *encoder,
                                 int16_t diff) {
  size_t length = get_amplitude_length(diff);
  write_huffman_code(self, buffer, symbols, encoder, length);
  write_amplitude(self, buffer, length, diff);
}

static void write_ac_coefficient(UtJpegEncoder *self, UtObject *buffer,
                                 UtObject *symbols, UtObject *encoder,
                                 size_t run_length, int16_t coefficient) {
  size_t length = get_amplitude_length(coefficient);
  write_huffman_code(self, buffer, symbols, encoder,
                     (run_length << 4) | length);
  write_amplitude(self, buffer, length, coefficient);
}

static void write_eob(UtJpegEncoder *self, UtObject *buffer, UtObject *symbols,
                      UtObject *encoder) {
  write_huffman_code(self, buffer, symbols, encoder, 0);
}

static void write_marker(UtObject *buffer, uint8_t value) {
  ut_uint8_list_append(buffer, 0xff);
  ut_uint8_list_append(buffer, value);
}

static void write_start_of_image(UtJpegEncoder *self) {
  UtObjectRef soi = ut_uint8_list_new();
  write_marker(soi, 0xd8);
  ut_output_stream_write(self->output_stream, soi);
}

static void write_app0(UtJpegEncoder *self) {
  uint8_t jpeg_version_major = 1;
  uint8_t jpeg_version_minor = 1;
  uint8_t density_units = 0;
  uint16_t x_density = 1;
  uint16_t y_density = 1;
  uint8_t thumbnail_width = 0;
  uint8_t thumbnail_height = 0;
  UtObjectRef thumbnail_data = ut_uint8_list_new();

  size_t length = 16 + ut_list_get_length(thumbnail_data);

  UtObjectRef app0 = ut_uint8_list_new();
  write_marker(app0, 0xe0);
  ut_uint8_list_append_uint16_be(app0, length);
  ut_uint8_list_append(app0, 'J');
  ut_uint8_list_append(app0, 'F');
  ut_uint8_list_append(app0, 'I');
  ut_uint8_list_append(app0, 'F');
  ut_uint8_list_append(app0, '\0');
  ut_uint8_list_append(app0, jpeg_version_major);
  ut_uint8_list_append(app0, jpeg_version_minor);
  ut_uint8_list_append(app0, density_units);
  ut_uint8_list_append_uint16_be(app0, x_density);
  ut_uint8_list_append_uint16_be(app0, y_density);
  ut_uint8_list_append(app0, thumbnail_width);
  ut_uint8_list_append(app0, thumbnail_height);
  ut_list_append_list(app0, thumbnail_data);
  ut_output_stream_write(self->output_stream, app0);
}

static void write_define_quantization_tables(UtJpegEncoder *self) {
  size_t length = 2;
  for (size_t i = 0; i < 4; i++) {
    if (self->quantization_tables[i] != NULL) {
      length += 65;
    }
  }

  UtObjectRef dqt = ut_uint8_list_new();
  write_marker(dqt, 0xdb);
  ut_uint8_list_append_uint16_be(dqt, length);
  for (size_t i = 0; i < 4; i++) {
    UtObject *quantization_table = self->quantization_tables[i];
    if (quantization_table != NULL) {
      uint8_t precision = 0;
      uint8_t destination = i;

      ut_uint8_list_append(dqt, precision << 4 | destination);
      const uint8_t *quantization_table_data =
          ut_uint8_list_get_data(self->quantization_tables[i]);
      for (size_t i = 0; i < 64; i++) {
        ut_uint8_list_append(dqt,
                             quantization_table_data[self->data_unit_order[i]]);
      }
    }
  }
  ut_output_stream_write(self->output_stream, dqt);
}

static void write_start_of_frame(UtJpegEncoder *self) {
  size_t image_width = ut_jpeg_image_get_width(self->image);
  size_t image_height = ut_jpeg_image_get_height(self->image);
  size_t n_components = ut_jpeg_image_get_n_components(self->image);

  uint8_t precision = 8;

  size_t length = 8 + 3 * n_components;

  UtObjectRef sof = ut_uint8_list_new();
  write_marker(sof, 0xc0);
  ut_uint8_list_append_uint16_be(sof, length);
  ut_uint8_list_append(sof, precision);
  ut_uint8_list_append_uint16_be(sof, image_height);
  ut_uint8_list_append_uint16_be(sof, image_width);
  ut_uint8_list_append(sof, n_components);
  for (size_t i = 0; i < n_components; i++) {
    uint8_t id = i;
    uint8_t horizontal_sampling_factor = 1;
    uint8_t vertical_sampling_factor = 1;
    uint8_t quantization_table = 0; // FIXME

    ut_uint8_list_append(sof, id);
    ut_uint8_list_append(sof, horizontal_sampling_factor << 4 |
                                  vertical_sampling_factor);
    ut_uint8_list_append(sof, quantization_table);
  }
  ut_output_stream_write(self->output_stream, sof);
}

static void write_huffman_table(UtObject *buffer, uint8_t class,
                                uint8_t destination, UtObject *symbols,
                                UtObject *encoder) {
  size_t symbols_length = ut_list_get_length(symbols);

  ut_uint8_list_append(buffer, class << 4 | destination);
  for (size_t length = 1; length <= 16; length++) {
    uint8_t count = 0;
    for (size_t i = 0; i < symbols_length; i++) {
      uint16_t code;
      size_t code_width;
      ut_huffman_encoder_get_code(encoder, i, &code, &code_width);
      if (code_width == length) {
        assert(count < 255);
        count++;
      }
    }
    ut_uint8_list_append(buffer, count);
  }
  for (size_t length = 1; length <= 16; length++) {
    for (size_t i = 0; i < symbols_length; i++) {
      uint16_t code;
      size_t code_width;
      ut_huffman_encoder_get_code(encoder, i, &code, &code_width);
      if (code_width == length) {
        uint8_t symbol = ut_uint8_list_get_element(symbols, i);
        ut_uint8_list_append(buffer, symbol);
      }
    }
  }
}

static void write_define_huffman_tables(UtJpegEncoder *self) {
  size_t length = 2;
  for (size_t i = 0; i < 2; i++) {
    if (self->dc_encoders[i] != NULL) {
      length += 17 + ut_list_get_length(self->dc_symbols[i]);
    }
    if (self->ac_encoders[i] != NULL) {
      length += 17 + ut_list_get_length(self->ac_symbols[i]);
    }
  }

  UtObjectRef dht = ut_uint8_list_new();
  write_marker(dht, 0xc4);
  ut_uint8_list_append_uint16_be(dht, length);
  for (size_t i = 0; i < 2; i++) {
    if (self->dc_encoders[i] != NULL) {
      write_huffman_table(dht, 0, i, self->dc_symbols[i], self->dc_encoders[i]);
    }
  }
  for (size_t i = 0; i < 2; i++) {
    if (self->ac_encoders[i] != NULL) {
      write_huffman_table(dht, 1, i, self->ac_symbols[i], self->ac_encoders[i]);
    }
  }
  ut_output_stream_write(self->output_stream, dht);
}

static void write_start_of_scan(UtJpegEncoder *self) {
  size_t image_width = ut_jpeg_image_get_width(self->image);
  size_t image_height = ut_jpeg_image_get_height(self->image);
  size_t n_components = ut_jpeg_image_get_n_components(self->image);

  size_t length = 3 + 5 * n_components;

  UtObjectRef sos = ut_uint8_list_new();
  write_marker(sos, 0xda);
  ut_uint8_list_append_uint16_be(sos, length);
  ut_uint8_list_append(sos, n_components);
  for (size_t i = 0; i < n_components; i++) {
    uint8_t component_selector = i; // FIXME
    uint8_t dc_table = 0;           // FIXME
    uint8_t ac_table = 0;           // FIXME
    uint8_t selection_start = 0;
    uint8_t selection_end = 63;
    uint8_t successive_approximation = 0;

    ut_uint8_list_append(sos, component_selector);
    ut_uint8_list_append(sos, dc_table << 4 | ac_table);
    ut_uint8_list_append(sos, selection_start);
    ut_uint8_list_append(sos, selection_end);
    ut_uint8_list_append(sos, successive_approximation);
  }

  size_t width_in_data_units = (image_width + 7) / 8;
  size_t height_in_data_units = (image_height + 7) / 8;
  int16_t previous_dc = 0;
  for (size_t component = 0; component < n_components; component++) {
    uint8_t quantization_table_index = 0; // FIXME
    uint8_t dc_table = 0;                 // FIXME
    uint8_t ac_table = 0;                 // FIXME

    UtObject *quantization_table =
        self->quantization_tables[quantization_table_index];
    UtObject *dc_symbols = self->dc_symbols[dc_table];
    UtObject *dc_encoder = self->dc_encoders[dc_table];
    UtObject *ac_symbols = self->ac_symbols[ac_table];
    UtObject *ac_encoder = self->ac_encoders[ac_table];

    const uint8_t *quantization_table_data =
        ut_uint8_list_get_data(quantization_table);

    for (size_t y = 0; y < height_in_data_units; y++) {
      for (size_t x = 0; x < width_in_data_units; x++) {
        int16_t data_unit[64], coefficients[64];
        float encoded_data_unit[64];

        // Copy values from image data into data unit.
        create_data_unit(self, x, y, component, data_unit);

        // Perform the discrete cosine transform on the data.
        jpeg_dct(self->dct_alpha, self->dct_cos, data_unit, encoded_data_unit);

        // Quantize coefficients and put into zigzag order.
        for (size_t i = 0; i < 64; i++) {
          uint8_t j = self->data_unit_order[i];
          coefficients[i] =
              round(encoded_data_unit[j] / (float)quantization_table_data[j]);
        }

        // Write out coefficients.
        int16_t dc = coefficients[0];
        int16_t diff = dc - previous_dc;
        previous_dc = dc;
        write_dc_coefficient(self, sos, dc_symbols, dc_encoder, diff);
        for (size_t i = 1; i < 64;) {
          // Count number of zeros before the next coefficient.
          size_t run_length = 0;
          while (coefficients[i + run_length] == 0 && i + run_length < 64) {
            run_length++;
          }

          if (i + run_length >= 64) {
            write_eob(self, sos, ac_symbols, ac_encoder);
            i = 64;
          } else if (run_length <= 15) {
            write_ac_coefficient(self, sos, ac_symbols, ac_encoder, run_length,
                                 coefficients[i + run_length]);
            i += run_length + 1;
          } else {
            write_ac_coefficient(self, sos, ac_symbols, ac_encoder, 15, 0);
            i += 16;
          }
        }
      }
    }
  }

  // Write any partially complete byte.
  end_bits(self, sos);

  // FIXME: Write data unit bytes as they are written.
  ut_output_stream_write(self->output_stream, sos);
}

static void write_end_of_image(UtJpegEncoder *self) {
  UtObjectRef eoi = ut_uint8_list_new();
  write_marker(eoi, 0xd9);
  ut_output_stream_write(self->output_stream, eoi);
}

static void ut_jpeg_encoder_cleanup(UtObject *object) {
  UtJpegEncoder *self = (UtJpegEncoder *)object;
  ut_object_unref(self->image);
  ut_object_unref(self->output_stream);
  for (size_t i = 0; i < 4; i++) {
    ut_object_unref(self->quantization_tables[i]);
  }
  for (size_t i = 0; i < 2; i++) {
    ut_object_unref(self->dc_symbols[i]);
    ut_object_unref(self->dc_encoders[i]);
    ut_object_unref(self->ac_symbols[i]);
    ut_object_unref(self->ac_encoders[i]);
  }
}

static UtObjectInterface object_interface = {
    .type_name = "UtJpegEncoder", .cleanup = ut_jpeg_encoder_cleanup};

UtObject *ut_jpeg_encoder_new(UtObject *image, UtObject *output_stream) {
  UtObject *object = ut_object_new(sizeof(UtJpegEncoder), &object_interface);
  UtJpegEncoder *self = (UtJpegEncoder *)object;
  self->image = ut_object_ref(image);
  self->output_stream = ut_object_ref(output_stream);

  build_tables(self);
  jpeg_build_data_unit_order(self->data_unit_order);
  jpeg_build_dct_values(self->dct_alpha, self->dct_cos);

  return object;
}

void ut_jpeg_encoder_encode(UtObject *object) {
  assert(ut_object_is_jpeg_encoder(object));
  UtJpegEncoder *self = (UtJpegEncoder *)object;

  write_start_of_image(self);
  write_app0(self);
  write_define_quantization_tables(self);
  write_start_of_frame(self);
  write_define_huffman_tables(self);
  write_start_of_scan(self);
  write_end_of_image(self);
}

bool ut_object_is_jpeg_encoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
