#include <assert.h>
#include <math.h>
#include <stdint.h>

#include "ut-jpeg-huffman-decoder.h"
#include "ut-jpeg-scan-component.h"
#include "ut-jpeg.h"
#include "ut.h"

// https://www.w3.org/Graphics/JPEG/itu-t81.pdf
// https://www.w3.org/Graphics/JPEG/jfif3.pdf

#define MAX_QUANTIZATION_TABLES 4
#define MAX_SCAN_COMPONENTS 4
#define MAX_HUFFMAN_TABLES 4

typedef enum {
  DECODER_STATE_MARKER,
  DECODER_STATE_DEFINE_QUANTIZATION_TABLE,
  DECODER_STATE_DEFINE_NUMBER_OF_LINES,
  DECODER_STATE_DEFINE_RESTART_INTERVAL,
  DECODER_STATE_EXPAND_REFERENCE_COMPONENT,
  DECODER_STATE_START_OF_FRAME,
  DECODER_STATE_DEFINE_HUFFMAN_TABLE,
  DECODER_STATE_DEFINE_ARITHMETIC_CODING,
  DECODER_STATE_START_OF_SCAN,
  DECODER_STATE_SCAN,
  DECODER_STATE_APP0,
  DECODER_STATE_APPLICATION_DATA,
  DECODER_STATE_COMMENT,
  DECODER_STATE_DONE,
  DECODER_STATE_ERROR
} DecoderState;

typedef enum {
  DECODE_MODE_BASELINE_DCT,
  DECODE_MODE_EXTENDED_DCT,
  DECODE_MODE_PROGRESSIVE_DCT,
  DECODE_MODE_LOSSLESS
} DecodeMode;

typedef struct {
  // ID assigned to this component.
  uint8_t id;

  // Index in the image.
  uint8_t index;

  // Sampling factors.
  uint8_t horizontal_sampling_factor;
  uint8_t vertical_sampling_factor;

  // The index of the quantization table used by this component.
  size_t quantization_table_selector;

  // Encoded image coefficients.
  UtObject *coefficients;
} JpegComponent;

typedef struct {
  UtObject object;

  // Input stream being read.
  UtObject *input_stream;

  // Callback to notify when complete.
  UtObject *callback_object;
  UtJpegDecodeCallback callback;

  // Current state of the decoder.
  DecoderState state;

  // Mode used to decode image.
  DecodeMode mode;

  // Number of bits of precision.
  size_t precision;

  // True if using differential.
  bool differential;

  // True if using arithmetic coding.
  bool arithmetic_coding;

  // True if using hierarchial progression.
  bool hierarchial_progression;

  // Tables for coefficient quantization values.
  UtObject *quantization_tables[MAX_QUANTIZATION_TABLES];

  // Components provided in image.
  JpegComponent components[MAX_SCAN_COMPONENTS];

  // Width of an MCU in data units.
  size_t mcu_width;

  // Height of an MCU in data units.
  size_t mcu_height;

  // Number of MCUs that make up the width of the image.
  size_t width_in_mcus;

  // Number of MCUs that make up the height of the image.
  size_t height_in_mcus;

  // Huffman/Arithmetic decoders for DC coefficients.
  UtObject *dc_decoders[MAX_HUFFMAN_TABLES];

  // Maps from DC Huffman symbols to coefficent values.
  UtObject *dc_tables[MAX_HUFFMAN_TABLES];

  // Huffman/Arithmentic decoders for AC coefficients.
  UtObject *ac_decoders[MAX_HUFFMAN_TABLES];

  // Maps from AC Huffman symbols to coefficent values.
  UtObject *ac_tables[MAX_HUFFMAN_TABLES];

  // Order that data unit values are written.
  uint8_t data_unit_order[64];

  // Precalculated cosines for DCT.
  float dct_alpha[8];
  float dct_cos[64];

  // Stream of scan data.
  UtObject *scan_stream;

  // Scan decoder (Huffman/Aritmetic).
  UtObject *scan_decoder;

  // Density information;
  UtJpegDensityUnits density_units;
  uint16_t horizontal_pixel_density;
  uint16_t vertical_pixel_density;

  // Image thumbnail.
  uint16_t thumbnail_width;
  uint16_t thumbnail_height;
  UtObject *thumbnail_data;

  // Image comment.
  char *comment;

  // Final image object.
  UtObject *image;

  // Error that occurred during decoding.
  UtObject *error;
} UtJpegDecoder;

static void notify_complete(UtJpegDecoder *self) {
  ut_input_stream_close(self->input_stream);
  if (self->callback_object != NULL) {
    self->callback(self->callback_object);
  }
}

__attribute__((format(printf, 2, 3))) static void
set_error(UtJpegDecoder *self, const char *format, ...) {
  if (self->state == DECODER_STATE_ERROR) {
    return;
  }

  ut_object_unref(self->image);
  self->image = NULL;
  va_list ap;
  va_start(ap, format);
  ut_cstring_ref description = ut_cstring_new_vprintf(format, ap);
  va_end(ap);
  self->error = ut_jpeg_error_new(description);
  self->state = DECODER_STATE_ERROR;
  notify_complete(self);
}

static void set_done(UtJpegDecoder *self) {
  self->state = DECODER_STATE_DONE;
  notify_complete(self);
}

// Get the component with the given ID.
static JpegComponent *find_component(UtJpegDecoder *self, uint8_t id) {
  size_t n_components = ut_jpeg_image_get_n_components(self->image);
  for (size_t i = 0; i < n_components; i++) {
    if (self->components[i].id == id) {
      return &self->components[i];
    }
  }

  return NULL;
}

#if 0
static uint8_t clamp(float value) {
  if (value < 0.0) {
    return 0;
  } else if (value > 255.0) {
    return 255;
  } else {
    return floor(value);
  }
}

static void ycbcr_to_rgb(uint8_t *pixel) {
  float Y = pixel[0];
  float Cb = pixel[1] - 128.0;
  float Cr = pixel[2] - 128.0;

  pixel[0] = clamp(Y + 1.402 * Cr);
  pixel[1] = clamp(Y - 0.344136 * Cb - 0.714136 * Cr);
  pixel[2] = clamp(Y + 1.772 * Cb);
}

// Process a received data unit.
static void process_data_unit(UtJpegDecoder *self) {
  uint16_t image_width = ut_jpeg_image_get_width(self->image);
  uint16_t image_height = ut_jpeg_image_get_height(self->image);
  size_t n_components = ut_jpeg_image_get_n_components(self->image);
  size_t row_stride = image_width * n_components;
  uint8_t *image_data =
      ut_uint8_list_get_writable_data(ut_jpeg_image_get_data(self->image));

  // Get position of current MCU in image.
  size_t mcu_x = (self->mcu_count % self->width_in_mcus) * self->mcu_width * 8;
  size_t mcu_y = (self->mcu_count / self->width_in_mcus) * self->mcu_height * 8;

  // Get position of current data unit in image.
  JpegComponent *component = self->scan_components[self->scan_component_index];
  size_t data_unit_x =
      mcu_x +
      (component->data_unit_count % component->horizontal_sampling_factor) * 8;
  size_t data_unit_y =
      mcu_y +
      (component->data_unit_count / component->horizontal_sampling_factor) * 8;

  // Do inverse DCT on data unit.
  int16_t *encoded_data_unit =
      ut_int16_list_get_writable_data(component->coefficients);
  int16_t decoded_data_unit[64];
  jpeg_inverse_dct(self->dct_alpha, self->dct_cos, encoded_data_unit,
                   decoded_data_unit);

  // Check if this is the final component being written.
  bool last_component =
      self->scan_component_index == n_components - 1 ||
      self->scan_components[self->scan_component_index + 1] == NULL;

  size_t sample_offset = 1 << (self->precision - 1);
  size_t sample_max = (1 << self->precision) - 1;

  // For now convert 12 bit samples to 8 bit.
  size_t sample_shift = self->precision - 8;

  // Write values into image.
  size_t sample_width = self->mcu_width / component->horizontal_sampling_factor;
  size_t sample_height = self->mcu_height / component->vertical_sampling_factor;
  for (size_t y = 0; y < 8; y++) {
    size_t sy = data_unit_y + y * sample_height;
    for (size_t x = 0; x < 8; x++) {
      size_t sx = data_unit_x + x * sample_width;

      int16_t sample = decoded_data_unit[(y * 8) + x] + sample_offset;
      if (sample < 0) {
        sample = 0;
      } else if (sample > sample_max) {
        sample = sample_max;
      }

      for (size_t v = 0; v < sample_height; v++) {
        size_t py = sy + v;
        if (py >= image_height) {
          break;
        }
        for (size_t u = 0; u < sample_width; u++) {
          size_t px = sx + u;
          if (px >= image_width) {
            break;
          }

          size_t pixel_index = (py * row_stride) + (px * n_components);
          image_data[pixel_index + component->index] = sample >> sample_shift;
          // Last channel placed - do color conversion.
          if (last_component && n_components == 3) {
            ycbcr_to_rgb(image_data + pixel_index);
          }
        }
      }
    }
  }
}
#endif

static void handle_restart(UtJpegDecoder *self, uint8_t count) {
  if (true) {
    set_error(self, "JPEG restart not supported");
    return;
  }

  self->state = DECODER_STATE_MARKER;
}

static void handle_start_of_image(UtJpegDecoder *self) {
  self->state = DECODER_STATE_MARKER;
}

static void handle_end_of_image(UtJpegDecoder *self) { set_done(self); }

static void decode_jfif(UtJpegDecoder *self, UtObject *data) {
  size_t data_length = ut_list_get_length(data);

  if (data_length < 9) {
    set_error(self, "Insufficient data for JPEG JFIF APP0");
    return;
  }

  uint8_t jpeg_version_major = ut_uint8_list_get_element(data, 0);
  uint8_t jpeg_version_minor = ut_uint8_list_get_element(data, 1);
  if (jpeg_version_major != 1) {
    set_error(self, "Unsupported JPEG version %d.%d", jpeg_version_major,
              jpeg_version_minor);
    return;
  }

  self->density_units = ut_uint8_list_get_element(data, 2);
  if (self->density_units > 2) {
    set_error(self, "Invalid JPEG density units %d", self->density_units);
    return;
  }
  self->horizontal_pixel_density = ut_uint8_list_get_uint16_be(data, 3);
  self->vertical_pixel_density = ut_uint8_list_get_uint16_be(data, 5);
  self->thumbnail_width = ut_uint8_list_get_element(data, 7);
  self->thumbnail_height = ut_uint8_list_get_element(data, 8);
  size_t thumbnail_size = self->thumbnail_width * self->thumbnail_height * 3;
  if (data_length < 9 + thumbnail_size) {
    set_error(self, "Insufficient space for JPEG thumbnail");
    return;
  }
  ut_object_unref(self->thumbnail_data);
  self->thumbnail_data = thumbnail_size > 0 ? ut_uint8_array_new() : NULL;
  for (size_t i = 0; i < thumbnail_size; i++) {
    ut_uint8_list_append(self->thumbnail_data,
                         ut_uint8_list_get_element(data, 9 + i));
  }
}

static void decode_jfxx(UtJpegDecoder *self, UtObject *data) {
  size_t data_length = ut_list_get_length(data);

  if (data_length < 1) {
    set_error(self, "Insufficient data for JPEG JFXX APP0");
    return;
  }

  uint8_t extension_code = ut_uint8_list_get_element(data, 0);

  switch (extension_code) {
  case 0x10:
    // FIXME: Thumbnail is JPEG
    break;
  case 0x11:
    // FIXME: Thumbnail one byte per pixel (palette)
    break;
  case 0x13:
    // FIXME: Thumbnail three bytes per pixel (RGB)
    break;
  }
}

static size_t decode_app0(UtJpegDecoder *self, UtObject *data) {
  size_t data_length = ut_list_get_length(data);

  if (data_length < 2) {
    return 0;
  }
  uint16_t length = ut_uint8_list_get_uint16_be(data, 0);
  if (data_length < length) {
    return 0;
  }

  // Ignore if insufficient space for JFIF/JFXX identifier - some unknown APP0
  // block.
  if (length < 7) {
    return length;
  }

  char identifier[5];
  for (size_t i = 0; i < 5; i++) {
    identifier[i] = ut_uint8_list_get_element(data, 2 + i);
  }
  if (identifier[4] != '\0') {
    return length;
  }
  UtObjectRef app0_data = ut_list_get_sublist(data, 7, length - 7);

  if (ut_cstring_equal(identifier, "JFIF")) {
    decode_jfif(self, app0_data);
  } else if (ut_cstring_equal(identifier, "JFXX")) {
    decode_jfxx(self, app0_data);
  }

  self->state = DECODER_STATE_MARKER;

  return length;
}

static size_t decode_application_data(UtJpegDecoder *self, UtObject *data) {
  size_t data_length = ut_list_get_length(data);

  if (data_length < 2) {
    return 0;
  }
  uint16_t length = ut_uint8_list_get_uint16_be(data, 0);
  if (data_length < length) {
    return 0;
  }

  self->state = DECODER_STATE_MARKER;

  return length;
}

static size_t decode_define_quantization_table(UtJpegDecoder *self,
                                               UtObject *data) {
  size_t data_length = ut_list_get_length(data);

  size_t offset = 0;
  if (data_length < 2) {
    return 0;
  }
  uint16_t length = ut_uint8_list_get_uint16_be(data, offset);
  offset += 2;
  if (data_length < length) {
    return 0;
  }

  while (offset < length) {
    if (length < offset + 65) {
      set_error(self, "Invalid JPEG quantization table size");
      return length;
    }

    uint8_t precision_and_destination =
        ut_uint8_list_get_element(data, offset++);
    uint8_t precision = precision_and_destination >> 4;
    uint8_t destination = precision_and_destination & 0xf;

    if (precision != 0) {
      set_error(self, "Invalid JPEG quantization table precision");
      return length;
    }
    if (destination > 3) {
      set_error(self, "Invalid JPEG quantization table destination");
      return length;
    }

    // Table is in same zig-zag order as the data units.
    ut_object_unref(self->quantization_tables[destination]);
    self->quantization_tables[destination] = ut_uint8_array_new_sized(64);
    uint8_t *quantization_table_data =
        ut_uint8_list_get_writable_data(self->quantization_tables[destination]);
    for (size_t i = 0; i < 64; i++) {
      quantization_table_data[self->data_unit_order[i]] =
          ut_uint8_list_get_element(data, offset++);
    }
  }

  self->state = DECODER_STATE_MARKER;

  return length;
}

static bool supported_precision(UtJpegDecoder *self, uint8_t precision) {
  switch (self->mode) {
  case DECODE_MODE_BASELINE_DCT:
    return precision == 8;
  case DECODE_MODE_EXTENDED_DCT:
  case DECODE_MODE_PROGRESSIVE_DCT:
    return precision == 8 || precision == 12;
  case DECODE_MODE_LOSSLESS:
    return precision >= 2 && precision <= 16;
  default:
    return false;
  }
}

static size_t decode_start_of_frame(UtJpegDecoder *self, UtObject *data) {
  size_t data_length = ut_list_get_length(data);

  if (data_length < 2) {
    return 0;
  }
  uint16_t length = ut_uint8_list_get_uint16_be(data, 0);
  if (data_length < length) {
    return 0;
  }

  if (length < 8) {
    set_error(self, "Insufficient space for JPEG start of frame");
    return length;
  }
  uint8_t precision = ut_uint8_list_get_element(data, 2);
  uint16_t height = ut_uint8_list_get_uint16_be(data, 3);
  uint16_t width = ut_uint8_list_get_uint16_be(data, 5);
  uint8_t n_components = ut_uint8_list_get_element(data, 7);

  // FIXME: Support height 0 - height determined by amount of data
  if (width == 0 || height == 0) {
    set_error(self, "Invalid JPEG width");
    return length;
  }

  if (n_components < 1 || n_components > MAX_SCAN_COMPONENTS) {
    set_error(self, "Unsupported number of JPEG components %d", n_components);
    return length;
  }

  if (length < 8 + 3 * n_components) {
    set_error(self, "Insufficient space for JPEG start of frame");
    return length;
  }
  size_t offset = 8;
  uint8_t mcu_width = 0;
  uint8_t mcu_height = 0;
  for (size_t i = 0; i < n_components; i++) {
    uint8_t id = ut_uint8_list_get_element(data, offset++);
    uint8_t sampling_factor = ut_uint8_list_get_element(data, offset++);
    uint8_t horizontal_sampling_factor = sampling_factor >> 4;
    uint8_t vertical_sampling_factor = sampling_factor & 0xf;
    uint8_t quantization_table_selector =
        ut_uint8_list_get_element(data, offset++);

    for (size_t j = 0; j < i; j++) {
      if (self->components[j].id == id) {
        set_error(self, "Duplicate JPEG component ID");
        return length;
      }
    }
    if (horizontal_sampling_factor < 1 || horizontal_sampling_factor > 4 ||
        vertical_sampling_factor < 1 || vertical_sampling_factor > 4) {
      set_error(self, "Invalid JPEG sampling factor %dx%d for component %zi",
                horizontal_sampling_factor, vertical_sampling_factor, i);
      return length;
    }
    if (quantization_table_selector > 3) {
      set_error(self, "Invalid JPEG component quantization table");
      return length;
    }

    if (horizontal_sampling_factor == 3 || vertical_sampling_factor == 3) {
      set_error(self, "Unsupported sampling factor %dx%d for component %zi",
                horizontal_sampling_factor, vertical_sampling_factor, i);
      return length;
    }
    if (horizontal_sampling_factor > mcu_width) {
      mcu_width = horizontal_sampling_factor;
    }
    if (vertical_sampling_factor > mcu_height) {
      mcu_height = vertical_sampling_factor;
    }

    self->components[i].id = id;
    self->components[i].index = i;
    self->components[i].horizontal_sampling_factor = horizontal_sampling_factor;
    self->components[i].vertical_sampling_factor = vertical_sampling_factor;
    self->components[i].quantization_table_selector =
        quantization_table_selector;

    self->components[i].coefficients =
        ut_int16_array_new_sized(64 * 1000 * 1000); // FIXME: scaling factor
  }
  self->mcu_width = mcu_width;
  self->mcu_height = mcu_height;
  self->width_in_mcus = (width + (mcu_width * 8) - 1) / (mcu_width * 8);
  self->height_in_mcus = (height + (mcu_height * 8) - 1) / (mcu_height * 8);

  if (!supported_precision(self, precision)) {
    set_error(self, "Unsupported JPEG precision %d", precision);
    return length;
  }
  self->precision = precision;

  if (self->hierarchial_progression) {
    set_error(self, "Hierarchial progression JPEG not supported");
    return length;
  }

  UtObjectRef image_data =
      ut_uint8_array_new_sized(height * width * n_components);
  self->image = ut_jpeg_image_new(
      width, height, self->density_units, self->horizontal_pixel_density,
      self->vertical_pixel_density, n_components, image_data);

  self->state = DECODER_STATE_MARKER;

  return length;
}

static bool supported_huffman_table_class(UtJpegDecoder *self, uint8_t class) {
  switch (self->mode) {
  case DECODE_MODE_BASELINE_DCT:
  case DECODE_MODE_EXTENDED_DCT:
  case DECODE_MODE_PROGRESSIVE_DCT:
    return class <= 1;
  case DECODE_MODE_LOSSLESS:
    return class == 0;
  default:
    return false;
  }
}

static bool supported_huffman_table_destination(UtJpegDecoder *self,
                                                uint8_t destination) {
  switch (self->mode) {
  case DECODE_MODE_BASELINE_DCT:
    return destination <= 1;
  case DECODE_MODE_EXTENDED_DCT:
  case DECODE_MODE_PROGRESSIVE_DCT:
  case DECODE_MODE_LOSSLESS:
    return destination < MAX_HUFFMAN_TABLES;
  default:
    return false;
  }
}

static size_t decode_define_huffman_table(UtJpegDecoder *self, UtObject *data) {
  size_t data_length = ut_list_get_length(data);

  size_t offset = 0;
  if (data_length < 2) {
    return 0;
  }
  uint16_t length = ut_uint8_list_get_uint16_be(data, offset);
  offset += 2;
  if (data_length < length) {
    return 0;
  }

  while (offset < length) {
    if (length < offset + 17) {
      set_error(self, "Insufficient space for JPEG Huffman table header");
      return length;
    }

    uint8_t class_and_destination = ut_uint8_list_get_element(data, offset++);

    // Huffman code is defined by the number of codes of with 1-16.
    UtObjectRef code_widths = ut_uint8_list_new();
    size_t last_code_width = 0;
    for (size_t code_width = 1; code_width <= 16; code_width++) {
      size_t count = ut_uint8_list_get_element(data, offset++);
      if (count > 0) {
        for (size_t j = 0; j < count; j++) {
          ut_uint8_list_append(code_widths, code_width);
        }
        last_code_width = code_width;
      }
    }
    size_t symbols_length = ut_list_get_length(code_widths);

    // The final Huffman code (all ones) is reserved and not given in the above
    // table.
    ut_uint8_list_append(code_widths, last_code_width);

    uint8_t class = class_and_destination >> 4;
    uint8_t destination = class_and_destination & 0xf;

    if (!supported_huffman_table_class(self, class)) {
      set_error(self, "Unsupported JPEG Huffman table class");
      return offset;
    }
    if (!supported_huffman_table_destination(self, destination)) {
      set_error(self, "Unsupported JPEG Huffman table destination");
      return offset;
    }

    if (length < offset + symbols_length) {
      set_error(self, "Insufficient space for JPEG Huffman table data");
      return offset;
    }
    UtObjectRef symbols = ut_list_get_sublist(data, offset, symbols_length);
    offset += symbols_length;

    UtObjectRef decoder = ut_huffman_decoder_new_canonical(code_widths);
    if (ut_object_implements_error(decoder)) {
      set_error(self, "Invalid JPEG Huffman table: %s",
                ut_error_get_description(decoder));
      return offset;
    }
    if (class == 0) {
      ut_object_unref(self->dc_decoders[destination]);
      self->dc_decoders[destination] = ut_object_ref(decoder);
      ut_object_unref(self->dc_tables[destination]);
      self->dc_tables[destination] = ut_list_copy(symbols);
    } else {
      ut_object_unref(self->ac_decoders[destination]);
      self->ac_decoders[destination] = ut_object_ref(decoder);
      ut_object_unref(self->ac_tables[destination]);
      self->ac_tables[destination] = ut_list_copy(symbols);
    }
  }

  self->state = DECODER_STATE_MARKER;

  return length;
}

static bool supported_arithmetic_table_class(UtJpegDecoder *self,
                                             uint8_t class) {
  switch (self->mode) {
  case DECODE_MODE_BASELINE_DCT:
    return false;
  case DECODE_MODE_EXTENDED_DCT:
  case DECODE_MODE_PROGRESSIVE_DCT:
    return class <= 1;
  case DECODE_MODE_LOSSLESS:
    return class == 0;
  default:
    return false;
  }
}

static bool supported_arithmetic_table_destination(UtJpegDecoder *self,
                                                   uint8_t destination) {
  switch (self->mode) {
  case DECODE_MODE_BASELINE_DCT:
    return false;
  case DECODE_MODE_EXTENDED_DCT:
  case DECODE_MODE_PROGRESSIVE_DCT:
  case DECODE_MODE_LOSSLESS:
    return destination <= 3;
  default:
    return false;
  }
}

static bool supported_arithmetic_conditioning_table_value(UtJpegDecoder *self,
                                                          uint8_t class,
                                                          uint8_t value) {
  switch (self->mode) {
  case DECODE_MODE_BASELINE_DCT:
    return false;
  case DECODE_MODE_EXTENDED_DCT:
  case DECODE_MODE_PROGRESSIVE_DCT:
    return class == 0 || (value >= 1 && value <= 63);
  case DECODE_MODE_LOSSLESS:
    return true;
  default:
    return false;
  }
}

static size_t decode_define_arithmetic_coding(UtJpegDecoder *self,
                                              UtObject *data) {
  size_t data_length = ut_list_get_length(data);

  size_t offset = 0;
  if (data_length < 2) {
    return 0;
  }
  uint16_t length = ut_uint8_list_get_uint16_be(data, offset);
  offset += 2;
  if (data_length < length) {
    return 0;
  }

  while (offset < length) {
    if (length < offset + 2) {
      set_error(self,
                "Insufficient space for JPEG Arithmetic conditioning table");
      return length;
    }

    uint8_t class_and_destination = ut_uint8_list_get_element(data, offset++);
    uint8_t conditioning_table_value =
        ut_uint8_list_get_element(data, offset++);

    uint8_t class = class_and_destination >> 4;
    uint8_t destination = class_and_destination & 0xf;

    if (!supported_arithmetic_table_class(self, class)) {
      set_error(self, "Unsupported JPEG Arithmetic table class");
      return offset;
    }
    if (!supported_arithmetic_table_destination(self, destination)) {
      set_error(self, "Unsupported JPEG Arithmetic table destination");
      return offset;
    }
    if (!supported_arithmetic_conditioning_table_value(
            self, class, conditioning_table_value)) {
      set_error(self, "Unsupported JPEG Arithmetic conditioning table value");
      return offset;
    }
  }

  if (true) {
    set_error(self, "JPEG arithmetic coding not supported");
    return 0;
  }

  self->state = DECODER_STATE_MARKER;

  return length;
}

static size_t decode_define_number_of_lines(UtJpegDecoder *self,
                                            UtObject *data) {
  size_t data_length = ut_list_get_length(data);

  if (data_length < 2) {
    return 0;
  }
  uint16_t length = ut_uint8_list_get_uint16_be(data, 0);
  if (length != 4) {
    set_error(self, "Invalid JPEG define number of lines length %d", length);
    return 0;
  }

  uint16_t number_of_lines = ut_uint8_list_get_uint16_be(data, 2);
  if (number_of_lines == 0) {
    set_error(self, "Invalid JPEG number of lines");
    return 0;
  }

  if (true) {
    set_error(self, "JPEG number of lines not supported");
    return 0;
  }

  self->state = DECODER_STATE_MARKER;

  return length;
}

static size_t decode_define_restart_interval(UtJpegDecoder *self,
                                             UtObject *data) {
  size_t data_length = ut_list_get_length(data);

  if (data_length < 2) {
    return 0;
  }
  uint16_t length = ut_uint8_list_get_uint16_be(data, 0);
  if (length != 4) {
    set_error(self, "Invalid JPEG define restart interval length %d", length);
    return 0;
  }

  /*uint16_t restart_interval =*/ut_uint8_list_get_uint16_be(data, 2);

  if (true) {
    set_error(self, "JPEG restart interval not supported");
    return 0;
  }

  self->state = DECODER_STATE_MARKER;

  return length;
}

static size_t decode_expand_reference_component(UtJpegDecoder *self,
                                                UtObject *data) {
  size_t data_length = ut_list_get_length(data);

  if (data_length < 2) {
    return 0;
  }
  uint16_t length = ut_uint8_list_get_uint16_be(data, 0);
  if (length != 3) {
    set_error(self, "Invalid JPEG expand reference component length %d",
              length);
    return 0;
  }

  uint8_t expand = ut_uint8_list_get_element(data, 2);
  uint8_t expand_horizontal = expand >> 4;
  uint8_t expand_vertical = expand & 0xf;

  if (expand_horizontal > 1 || expand_vertical > 1) {
    set_error(self,
              "Invalid expand value %dx%d in JPEG expand reference component",
              expand_horizontal, expand_vertical);
    return 0;
  }

  if (true) {
    set_error(self, "JPEG expand reference component not supported");
    return 0;
  }

  self->state = DECODER_STATE_MARKER;

  return length;
}

static bool supported_dc_table(UtJpegDecoder *self, uint8_t dc_table) {
  switch (self->mode) {
  case DECODE_MODE_BASELINE_DCT:
    return dc_table <= 1;
  case DECODE_MODE_EXTENDED_DCT:
  case DECODE_MODE_PROGRESSIVE_DCT:
  case DECODE_MODE_LOSSLESS:
    return dc_table <= 3;
  default:
    return false;
  }
}

static bool supported_ac_table(UtJpegDecoder *self, uint8_t ac_table) {
  switch (self->mode) {
  case DECODE_MODE_BASELINE_DCT:
    return ac_table <= 1;
  case DECODE_MODE_EXTENDED_DCT:
  case DECODE_MODE_PROGRESSIVE_DCT:
    return ac_table <= 3;
  case DECODE_MODE_LOSSLESS:
    return ac_table == 0;
  default:
    return false;
  }
}

static bool supported_scan_selection(UtJpegDecoder *self,
                                     uint8_t selection_start,
                                     uint8_t selection_end) {
  switch (self->mode) {
  case DECODE_MODE_BASELINE_DCT:
  case DECODE_MODE_EXTENDED_DCT:
    return selection_start == 0 && selection_end == 63;
  case DECODE_MODE_PROGRESSIVE_DCT:
    return selection_end <= 63 && selection_start <= selection_end;
  case DECODE_MODE_LOSSLESS:
    return (selection_start >= 1 && selection_start <= 7) && selection_end == 0;
  default:
    return false;
  }
}

static bool
supported_successive_approximation(UtJpegDecoder *self,
                                   uint8_t successive_approximation_high,
                                   uint8_t successive_approximation_low) {
  switch (self->mode) {
  case DECODE_MODE_BASELINE_DCT:
  case DECODE_MODE_EXTENDED_DCT:
    return successive_approximation_high == 0 &&
           successive_approximation_low == 0;
  case DECODE_MODE_PROGRESSIVE_DCT:
    return successive_approximation_high <= 13 &&
           successive_approximation_low <= 13;
  case DECODE_MODE_LOSSLESS:
    return successive_approximation_high == 0;
  default:
    return false;
  }
}

static size_t decode_start_of_scan(UtJpegDecoder *self, UtObject *data) {
  size_t data_length = ut_list_get_length(data);

  if (data_length < 3) {
    return 0;
  }
  size_t offset = 0;
  uint16_t length = ut_uint8_list_get_uint16_be(data, offset);
  offset += 2;
  if (data_length < length) {
    return 0;
  }

  size_t n_scan_components = ut_uint8_list_get_element(data, offset++);
  if (length < 6 + 2 * n_scan_components) {
    set_error(self, "Insufficient data for JPEG start of scan");
    return length;
  }
  if (n_scan_components < 1 || n_scan_components > MAX_SCAN_COMPONENTS) {
    set_error(self, "Unsupported number of JPEG scan components %zi",
              n_scan_components);
    return length;
  }

  UtObjectRef components = ut_list_new();
  for (size_t i = 0; i < n_scan_components; i++) {
    uint8_t component_selector = ut_uint8_list_get_element(data, offset++);
    uint8_t table_selectors = ut_uint8_list_get_element(data, offset++);
    uint8_t dc_table = table_selectors >> 4;
    uint8_t ac_table = table_selectors & 0xf;

    JpegComponent *component = find_component(self, component_selector);
    if (component == NULL) {
      set_error(self, "Invalid component selector in JPEG start of scan");
      return length;
    }

    if (!supported_dc_table(self, dc_table)) {
      set_error(self, "Invalid DC table selector in JPEG start of scan");
      return length;
    }
    if (!supported_ac_table(self, ac_table)) {
      set_error(self, "Invalid AC table selector in JPEG start of scan");
      return length;
    }

    UtObjectRef scan_component = ut_jpeg_scan_component_new(
        self->dc_decoders[dc_table], self->dc_tables[dc_table],
        self->ac_decoders[ac_table], self->ac_tables[ac_table],
        self->quantization_tables[component->quantization_table_selector],
        component->coefficients);
    ut_list_append(components, scan_component);
  }
  uint8_t selection_start = ut_uint8_list_get_element(data, offset++);
  uint8_t selection_end = ut_uint8_list_get_element(data, offset++);
  uint8_t successive_approximation = ut_uint8_list_get_element(data, offset++);
  uint8_t successive_approximation_high = successive_approximation >> 4;
  uint8_t successive_approximation_low = successive_approximation & 0xf;

  if (!supported_scan_selection(self, selection_start, selection_end)) {
    set_error(self, "Invalid scan selection range %d-%d in JPEG start of scan",
              selection_start, selection_end);
    return length;
  }
  if (!supported_successive_approximation(self, successive_approximation_high,
                                          successive_approximation_low) != 0) {
    set_error(self, "Invalid successive approximation in JPEG start of scan");
    return length;
  }

  if (self->mode == DECODE_MODE_PROGRESSIVE_DCT) {
    set_error(self, "Progressive JPEG not supported");
    return length;
  }
  if (self->mode == DECODE_MODE_LOSSLESS) {
    set_error(self, "Lossless JPEG not supported");
    return length;
  }

  if (self->differential) {
    set_error(self, "Differential JPEG not supported");
    return length;
  }
  if (self->arithmetic_coding) {
    set_error(self, "Arithmetic JPEG not supported");
    return length;
  }

  // Check have required decoders.
#if 0
  for (size_t i = 0; i < n_scan_components; i++) {
    if (selection_start == 0 && self->scan_components[i]->dc_decoder == NULL) {
      set_error(self, "Missing DC table in JPEG start of scan");
      return length;
    }
    if (selection_end > 0 && self->scan_components[i]->ac_decoder == NULL) {
      set_error(self, "Missing AC table in JPEG start of scan");
      return length;
    }
  }
#endif

  ut_object_unref(self->scan_stream);
  self->scan_stream = ut_writable_input_stream_new();
  self->scan_decoder = ut_jpeg_huffman_decoder_new(
      self->scan_stream, components, selection_start, selection_end);
  ut_jpeg_huffman_decoder_decode(self->scan_decoder);
  self->state = DECODER_STATE_SCAN;

  return length;
}

static size_t decode_scan(UtJpegDecoder *self, UtObject *data, bool complete) {
  // Look for marker at end of scan.
  size_t data_length = ut_list_get_length(data);
  size_t scan_length = data_length;
  bool scan_complete = false;
  for (size_t i = 0; i < data_length; i++) {
    if (ut_uint8_list_get_element(data, i) == 0xff) {
      if (i == data_length - 1) {
        if (complete) {
          scan_complete = true;
        } else {
          // Don't process a trailing 0xff - it might be a marker.
          scan_length = data_length - 1;
        }
      } else {
        if (ut_uint8_list_get_element(data, i + 1) != 0x00) {
          scan_length = i;
          scan_complete = true;
        }
      }
    }
  }

  UtObjectRef scan_data = ut_list_get_sublist(data, 0, scan_length);
  size_t n_used = ut_writable_input_stream_write(self->scan_stream, scan_data,
                                                 scan_complete);
  if (scan_complete) {
    n_used = scan_length;
    self->state = DECODER_STATE_MARKER;
  }

  return n_used;
}

static size_t decode_comment(UtJpegDecoder *self, UtObject *data) {
  size_t data_length = ut_list_get_length(data);

  if (data_length < 2) {
    return 0;
  }
  uint16_t length = ut_uint8_list_get_uint16_be(data, 0);
  if (data_length < length) {
    return 0;
  }

  UtObjectRef comment_data = ut_uint8_array_new();
  for (size_t i = 2; i < length; i++) {
    ut_uint8_list_append(comment_data, ut_uint8_list_get_element(data, i));
  }
  ut_uint8_list_append(comment_data, '\0');
  free(self->comment);
  self->comment = (char *)ut_uint8_list_take_data(comment_data);

  self->state = DECODER_STATE_MARKER;

  return length;
}

static size_t decode_marker(UtJpegDecoder *self, UtObject *data) {
  size_t data_length = ut_list_get_length(data);

  if (data_length < 2) {
    return 0;
  }
  uint8_t marker_signature = ut_uint8_list_get_element(data, 0);
  uint8_t marker_id = ut_uint8_list_get_element(data, 1);
  if (marker_signature != 0xff) {
    set_error(self, "Invalid JPEG marker signature");
    return 0;
  }

  switch (marker_id) {
  case 0xd0:
  case 0xd1:
  case 0xd2:
  case 0xd3:
  case 0xd4:
  case 0xd5:
  case 0xd6:
  case 0xd7:
    handle_restart(self, marker_id - 0xd0);
    break;
  case 0xd8:
    handle_start_of_image(self);
    break;
  case 0xd9:
    handle_end_of_image(self);
    break;
  case 0xda:
    self->state = DECODER_STATE_START_OF_SCAN;
    break;
  case 0xdb:
    self->state = DECODER_STATE_DEFINE_QUANTIZATION_TABLE;
    break;
  case 0xdc:
    self->state = DECODER_STATE_DEFINE_NUMBER_OF_LINES;
    break;
  case 0xdd:
    self->state = DECODER_STATE_DEFINE_RESTART_INTERVAL;
    break;
  case 0xde:
    self->hierarchial_progression = true;
    self->state = DECODER_STATE_START_OF_FRAME;
    break;
  case 0xdf:
    self->state = DECODER_STATE_EXPAND_REFERENCE_COMPONENT;
    break;
  case 0xc0:
    self->mode = DECODE_MODE_BASELINE_DCT;
    self->state = DECODER_STATE_START_OF_FRAME;
    break;
  case 0xc1:
    self->mode = DECODE_MODE_EXTENDED_DCT;
    self->state = DECODER_STATE_START_OF_FRAME;
    break;
  case 0xc2:
    self->mode = DECODE_MODE_PROGRESSIVE_DCT;
    self->state = DECODER_STATE_START_OF_FRAME;
    break;
  case 0xc3:
    self->mode = DECODE_MODE_LOSSLESS;
    self->state = DECODER_STATE_START_OF_FRAME;
    break;
  case 0xc4:
    self->state = DECODER_STATE_DEFINE_HUFFMAN_TABLE;
    break;
  case 0xc5:
    self->differential = true;
    self->mode = DECODE_MODE_EXTENDED_DCT;
    self->state = DECODER_STATE_START_OF_FRAME;
    break;
  case 0xc6:
    self->differential = true;
    self->mode = DECODE_MODE_PROGRESSIVE_DCT;
    self->state = DECODER_STATE_START_OF_FRAME;
    break;
  case 0xc7:
    self->differential = true;
    self->mode = DECODE_MODE_LOSSLESS;
    self->state = DECODER_STATE_START_OF_FRAME;
    break;
  case 0xc9:
    self->arithmetic_coding = true;
    self->mode = DECODE_MODE_EXTENDED_DCT;
    self->state = DECODER_STATE_START_OF_FRAME;
    break;
  case 0xca:
    self->arithmetic_coding = true;
    self->mode = DECODE_MODE_PROGRESSIVE_DCT;
    self->state = DECODER_STATE_START_OF_FRAME;
    break;
  case 0xcb:
    self->arithmetic_coding = true;
    self->mode = DECODE_MODE_LOSSLESS;
    self->state = DECODER_STATE_START_OF_FRAME;
    break;
  case 0xcc:
    self->state = DECODER_STATE_DEFINE_ARITHMETIC_CODING;
    break;
  case 0xcd:
    self->differential = true;
    self->arithmetic_coding = true;
    self->mode = DECODE_MODE_EXTENDED_DCT;
    self->state = DECODER_STATE_START_OF_FRAME;
    break;
  case 0xce:
    self->differential = true;
    self->arithmetic_coding = true;
    self->mode = DECODE_MODE_PROGRESSIVE_DCT;
    self->state = DECODER_STATE_START_OF_FRAME;
    break;
  case 0xcf:
    self->differential = true;
    self->arithmetic_coding = true;
    self->mode = DECODE_MODE_LOSSLESS;
    self->state = DECODER_STATE_START_OF_FRAME;
    break;
  case 0xe0:
    self->state = DECODER_STATE_APP0;
    break;
  case 0xe1:
  case 0xe2:
  case 0xe3:
  case 0xe4:
  case 0xe5:
  case 0xe6:
  case 0xe7:
  case 0xe8:
  case 0xe9:
  case 0xea:
  case 0xeb:
  case 0xec:
  case 0xed:
  case 0xee:
  case 0xef:
    self->state = DECODER_STATE_APPLICATION_DATA;
    break;
  case 0xfe:
    self->state = DECODER_STATE_COMMENT;
    break;
  default:
    set_error(self, "Unknown JPEG marker %02x", marker_id);
    break;
  }

  return 2;
}

static size_t read_cb(UtObject *object, UtObject *data, bool complete) {
  UtJpegDecoder *self = (UtJpegDecoder *)object;

  if (ut_object_implements_error(data)) {
    set_error(self, "Failed to read JPEG data: %s",
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
    case DECODER_STATE_MARKER:
      n_used = decode_marker(self, d);
      break;
    case DECODER_STATE_DEFINE_QUANTIZATION_TABLE:
      n_used = decode_define_quantization_table(self, d);
      break;
    case DECODER_STATE_DEFINE_NUMBER_OF_LINES:
      n_used = decode_define_number_of_lines(self, d);
      break;
    case DECODER_STATE_DEFINE_RESTART_INTERVAL:
      n_used = decode_define_restart_interval(self, d);
      break;
    case DECODER_STATE_EXPAND_REFERENCE_COMPONENT:
      n_used = decode_expand_reference_component(self, d);
      break;
    case DECODER_STATE_START_OF_FRAME:
      n_used = decode_start_of_frame(self, d);
      break;
    case DECODER_STATE_DEFINE_HUFFMAN_TABLE:
      n_used = decode_define_huffman_table(self, d);
      break;
    case DECODER_STATE_DEFINE_ARITHMETIC_CODING:
      n_used = decode_define_arithmetic_coding(self, d);
      break;
    case DECODER_STATE_START_OF_SCAN:
      n_used = decode_start_of_scan(self, d);
      break;
    case DECODER_STATE_SCAN:
      n_used = decode_scan(self, d, complete);
      break;
    case DECODER_STATE_APP0:
      n_used = decode_app0(self, d);
      break;
    case DECODER_STATE_APPLICATION_DATA:
      n_used = decode_application_data(self, d);
      break;
    case DECODER_STATE_COMMENT:
      n_used = decode_comment(self, d);
      break;
    case DECODER_STATE_ERROR:
    case DECODER_STATE_DONE:
      return offset;
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

static void done_cb(UtObject *object) {}

static void ut_jpeg_decoder_init(UtObject *object) {
  UtJpegDecoder *self = (UtJpegDecoder *)object;
  jpeg_build_data_unit_order(self->data_unit_order);
  jpeg_build_dct_values(self->dct_alpha, self->dct_cos);
}

static void ut_jpeg_decoder_cleanup(UtObject *object) {
  UtJpegDecoder *self = (UtJpegDecoder *)object;

  ut_input_stream_close(self->input_stream);

  ut_object_unref(self->input_stream);
  ut_object_weak_unref(&self->callback_object);
  for (size_t i = 0; i < MAX_QUANTIZATION_TABLES; i++) {
    ut_object_unref(self->quantization_tables[i]);
  }
  for (size_t i = 0; i < MAX_HUFFMAN_TABLES; i++) {
    ut_object_unref(self->dc_decoders[i]);
    ut_object_unref(self->dc_tables[i]);
    ut_object_unref(self->ac_decoders[i]);
    ut_object_unref(self->ac_tables[i]);
  }
  ut_object_unref(self->scan_stream);
  ut_object_unref(self->scan_decoder);
  for (size_t i = 0; i < MAX_SCAN_COMPONENTS; i++) {
    ut_object_unref(self->components[i].coefficients);
  }
  ut_object_unref(self->thumbnail_data);
  free(self->comment);
  ut_object_unref(self->image);
  ut_object_unref(self->error);
}

static UtObjectInterface object_interface = {.type_name = "UtJpegDecoder",
                                             .init = ut_jpeg_decoder_init,
                                             .cleanup =
                                                 ut_jpeg_decoder_cleanup};

UtObject *ut_jpeg_decoder_new(UtObject *input_stream) {
  UtObject *object = ut_object_new(sizeof(UtJpegDecoder), &object_interface);
  UtJpegDecoder *self = (UtJpegDecoder *)object;
  self->input_stream = ut_object_ref(input_stream);
  return object;
}

void ut_jpeg_decoder_decode(UtObject *object, UtObject *callback_object,
                            UtJpegDecodeCallback callback) {
  assert(ut_object_is_jpeg_decoder(object));
  UtJpegDecoder *self = (UtJpegDecoder *)object;

  assert(self->callback == NULL);
  assert(callback != NULL);

  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callback = callback;

  ut_input_stream_read(self->input_stream, object, read_cb);
}

UtObject *ut_jpeg_decoder_decode_sync(UtObject *object) {
  assert(ut_object_is_jpeg_decoder(object));
  UtJpegDecoder *self = (UtJpegDecoder *)object;

  UtObjectRef dummy_object = ut_null_new();
  ut_jpeg_decoder_decode(object, (UtObject *)dummy_object, done_cb);
  if (self->error != NULL) {
    return ut_object_ref(self->error);
  }

  return ut_object_ref(self->image);
}

UtObject *ut_jpeg_decoder_get_error(UtObject *object) {
  assert(ut_object_is_jpeg_decoder(object));
  UtJpegDecoder *self = (UtJpegDecoder *)object;
  return self->error;
}

UtObject *ut_jpeg_decoder_get_image(UtObject *object) {
  assert(ut_object_is_jpeg_decoder(object));
  UtJpegDecoder *self = (UtJpegDecoder *)object;
  return self->image;
}

bool ut_object_is_jpeg_decoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
