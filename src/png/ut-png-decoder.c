#include <assert.h>

#include "ut-png.h"
#include "ut.h"

typedef enum {
  DECODER_STATE_SIGNATURE,
  DECODER_STATE_ERROR,
  DECODER_STATE_CHUNK,
  DECODER_STATE_END
} DecoderState;

typedef enum {
  FILTER_TYPE_NONE,
  FILTER_TYPE_SUB,
  FILTER_TYPE_UP,
  FILTER_TYPE_AVERAGE,
  FILTER_TYPE_PAETH
} FilterType;

typedef struct {
  UtObject object;

  // Input stream being read.
  UtObject *input_stream;

  // Callback to notify when complete.
  UtObject *callback_object;
  UtPngDecodeCallback callback;

  // Current state of the decoder.
  DecoderState state;

  // Interlace method being used.
  UtPngInterlaceMethod interlace_method;

  // Image compression decoding.
  UtObject *image_data_decoder_input_stream;
  UtObject *image_data_decoder;

  // Scanline being decoded.
  size_t interlace_pass;
  size_t row_count;
  UtObject *previous_row;

  // Final image object.
  UtObject *image;

  // Error that occurred during decoding.
  UtObject *error;
} UtPngDecoder;

static uint32_t crc_table[256] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
    0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
    0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
    0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
    0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
    0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
    0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
    0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
    0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
    0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
    0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
    0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
    0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
    0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
    0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
    0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
    0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
    0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
    0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
    0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
    0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
    0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
    0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d};

static uint32_t crc32(UtObject *data, size_t offset, size_t length) {
  uint32_t c = 0xffffffff;
  const uint8_t *d = ut_uint8_list_get_data(data);
  if (d != NULL) {
    for (size_t i = 0; i < length; i++) {
      c = crc_table[(c ^ d[offset + i]) & 0xff] ^ (c >> 8);
    }
  } else {
    for (size_t i = 0; i < length; i++) {
      uint8_t value = ut_uint8_list_get_element(data, offset + i);
      c = crc_table[(c ^ value) & 0xff] ^ (c >> 8);
    }
  }
  return c ^ 0xffffffff;
}

static void notify_complete(UtPngDecoder *self) {
  ut_input_stream_close(self->input_stream);
  if (self->callback_object != NULL) {
    self->callback(self->callback_object);
  }
}

static void set_error(UtPngDecoder *self, const char *description) {
  if (self->state == DECODER_STATE_ERROR) {
    return;
  }

  self->error = ut_png_error_new(description);
  self->state = DECODER_STATE_ERROR;
  notify_complete(self);
}

static void set_error_take(UtPngDecoder *self, char *description) {
  set_error(self, description);
  free(description);
}

static void set_end(UtPngDecoder *self) {
  self->state = DECODER_STATE_END;
  notify_complete(self);
}

static size_t decode_signature(UtPngDecoder *self, UtObject *data) {
  if (ut_list_get_length(data) < 8) {
    return 0;
  }

  if (ut_uint8_list_get_element(data, 0) != 137 ||
      ut_uint8_list_get_element(data, 1) != 80 ||
      ut_uint8_list_get_element(data, 2) != 78 ||
      ut_uint8_list_get_element(data, 3) != 71 ||
      ut_uint8_list_get_element(data, 4) != 13 ||
      ut_uint8_list_get_element(data, 5) != 10 ||
      ut_uint8_list_get_element(data, 6) != 26 ||
      ut_uint8_list_get_element(data, 7) != 10) {
    set_error(self, "Invalid PNG signature");
    return 0;
  }

  self->state = DECODER_STATE_CHUNK;
  return 8;
}

static bool decode_color_type(uint8_t value, UtPngColorType *type) {
  switch (value) {
  case 0:
    *type = UT_PNG_COLOR_TYPE_GREYSCALE;
    return true;
  case 2:
    *type = UT_PNG_COLOR_TYPE_TRUECOLOR;
    return true;
  case 3:
    *type = UT_PNG_COLOR_TYPE_INDEXED_COLOR;
    return true;
  case 4:
    *type = UT_PNG_COLOR_TYPE_GREYSCALE_WITH_ALPHA;
    return true;
  case 6:
    *type = UT_PNG_COLOR_TYPE_TRUECOLOR_WITH_ALPHA;
    return true;
  default:
    return false;
  }
}

static bool is_valid_bit_depth(uint8_t type, uint8_t depth) {
  switch (type) {
  case UT_PNG_COLOR_TYPE_GREYSCALE:
    return depth == 1 || depth == 2 || depth == 4 || depth == 8 || depth == 16;
  case UT_PNG_COLOR_TYPE_TRUECOLOR:
    return depth == 8 || depth == 16;
  case UT_PNG_COLOR_TYPE_INDEXED_COLOR:
    return depth == 1 || depth == 2 || depth == 4 || depth == 8;
  case UT_PNG_COLOR_TYPE_GREYSCALE_WITH_ALPHA:
    return depth == 8 || depth == 16;
  case UT_PNG_COLOR_TYPE_TRUECOLOR_WITH_ALPHA:
    return depth == 8 || depth == 16;
  default:
    return false;
  }
}

static bool decode_interlace_method(uint8_t value,
                                    UtPngInterlaceMethod *method) {
  switch (value) {
  case 0:
    *method = UT_PNG_INTERLACE_METHOD_NONE;
    return true;
  case 1:
    *method = UT_PNG_INTERLACE_METHOD_ADAM7;
    return true;
  default:
    return false;
  }
}

static bool decode_filter_type(uint8_t value, FilterType *type) {
  switch (value) {
  case 0:
    *type = FILTER_TYPE_NONE;
    return true;
  case 1:
    *type = FILTER_TYPE_SUB;
    return true;
  case 2:
    *type = FILTER_TYPE_UP;
    return true;
  case 3:
    *type = FILTER_TYPE_AVERAGE;
    return true;
  case 4:
    *type = FILTER_TYPE_PAETH;
    return true;
  default:
    return false;
  }
}

static uint8_t paeth_filter(uint8_t x, int32_t a, int32_t b, int32_t c) {
  int32_t p = a + b - c;
  int32_t pa = p > a ? p - a : a - p;
  int32_t pb = p > b ? p - b : b - p;
  int32_t pc = p > c ? p - c : c - p;
  if (pa <= pb && pa <= pc) {
    return x + a;
  } else if (pb <= pc) {
    return x + b;
  } else {
    return x + c;
  }
}

static void filter_row(UtPngDecoder *self, FilterType filter,
                       UtObject *previous_row, UtObject *filtered_row,
                       UtObject *row) {
  size_t row_length = ut_list_get_length(filtered_row);
  uint8_t bit_depth = ut_png_image_get_bit_depth(self->image);
  size_t n_channels = ut_png_image_get_n_channels(self->image);
  const uint8_t *previous_row_data =
      previous_row != NULL ? ut_uint8_list_get_data(previous_row) : NULL;
  uint8_t *row_data = ut_uint8_list_get_writable_data(row);

  for (size_t offset = 0; offset < row_length; offset++) {
    // Calculate the filter inputs:
    //
    // +---+---+
    // | c | b |
    // +---+---+
    // | a | x |
    // +---+---+
    uint8_t x = ut_uint8_list_get_element(filtered_row, offset);
    int32_t a, b, c;
    size_t left_offset;
    if (bit_depth < 8) {
      left_offset = 1;
    } else {
      left_offset = n_channels * (bit_depth / 8);
    }
    if (offset >= left_offset) {
      a = row_data[offset - left_offset];
      c = previous_row_data != NULL ? previous_row_data[offset - left_offset]
                                    : 0;
    } else {
      a = c = 0;
    }
    b = previous_row_data != NULL ? previous_row_data[offset] : 0;

    // Reconstruct the pixel value.
    uint8_t recon_x;
    switch (filter) {
    default:
    case FILTER_TYPE_NONE:
      recon_x = x;
      break;
    case FILTER_TYPE_SUB:
      recon_x = x + a;
      break;
    case FILTER_TYPE_UP:
      recon_x = x + b;
      break;
    case FILTER_TYPE_AVERAGE:
      recon_x = x + (a + b) / 2;
      break;
    case FILTER_TYPE_PAETH:
      recon_x = paeth_filter(x, a, b, c);
      break;
    }

    row_data[offset] = recon_x;
  }
}

// Gets the dimensions of an Adam7 interlacing [pass] for an image of size
// [width]x[height].
static void get_adam7_dimensions(size_t width, size_t height, size_t pass,
                                 size_t *pass_width, size_t *pass_height) {
  switch (pass) {
  case 0:
    *pass_width = (width + 7) / 8;
    *pass_height = (height + 7) / 8;
    break;
  case 1:
    *pass_width = (width + 3) / 8;
    *pass_height = (height + 7) / 8;
    break;
  case 2:
    *pass_width = (width + 3) / 4;
    *pass_height = (height + 3) / 8;
    break;
  case 3:
    *pass_width = (width + 1) / 4;
    *pass_height = (height + 3) / 4;
    break;
  case 4:
    *pass_width = (width + 1) / 2;
    *pass_height = (height + 1) / 4;
    break;
  case 5:
    *pass_width = width / 2;
    *pass_height = (height + 1) / 2;
    break;
  case 6:
    *pass_width = width;
    *pass_height = height / 2;
    break;
  default:
    *pass_width = width;
    *pass_height = height;
    break;
  }
}

static uint8_t get_sample1(const uint8_t *row, size_t index) {
  return (row[index / 8] >> (7 - index % 8)) & 0x1;
}

static uint8_t get_sample2(const uint8_t *row, size_t index) {
  return (row[index / 4] >> 2 * (3 - index % 4)) & 0x3;
}

static uint8_t get_sample4(const uint8_t *row, size_t index) {
  return (row[index / 2] >> 4 * (1 - index % 2)) & 0xf;
}

static uint8_t get_sample8(const uint8_t *row, size_t index) {
  return row[index];
}

static void write_sample1(uint8_t *image_data, size_t row_stride, size_t index,
                          size_t y, uint8_t value) {
  uint8_t *row = image_data + (y * row_stride);
  uint8_t mask = (0x1 << (7 - index % 8)) ^ 0xff;
  row[index / 8] &= mask;
  row[index / 8] |= value << (7 - index % 8);
}

static void write_sample2(uint8_t *image_data, size_t row_stride, size_t index,
                          size_t y, uint8_t value) {
  uint8_t *row = image_data + (y * row_stride);
  uint8_t mask = (0x3 << 2 * (3 - index % 4)) ^ 0xff;
  row[index / 4] &= mask;
  row[index / 4] |= value << 2 * (3 - index % 4);
}

static void write_sample4(uint8_t *image_data, size_t row_stride, size_t index,
                          size_t y, uint8_t value) {
  uint8_t *row = image_data + (y * row_stride);
  uint8_t mask = (0xf << 4 * (1 - index % 2)) ^ 0xff;
  row[index / 2] &= mask;
  row[index / 2] |= value << 4 * (1 - index % 2);
}

static void write_sample8(uint8_t *image_data, size_t row_stride, size_t index,
                          size_t y, uint8_t value) {
  uint8_t *row = image_data + (y * row_stride);
  row[index] = value;
}

// Take interlace [row] of [row_width] pixels and write it into the final image.
static void apply_adam7_row(UtPngDecoder *self, UtObject *row,
                            size_t row_width) {
  uint32_t image_height = ut_png_image_get_height(self->image);
  uint32_t image_width = ut_png_image_get_width(self->image);
  uint8_t bit_depth = ut_png_image_get_bit_depth(self->image);
  size_t n_channels = ut_png_image_get_n_channels(self->image);
  size_t row_stride = ut_png_image_get_row_stride(self->image);
  uint8_t *image_data =
      ut_uint8_list_get_writable_data(ut_png_image_get_data(self->image));

  // Get the transform for the interlaced row and the size to fill.
  size_t x0, y0, dx, dy, fill_width, fill_height;
  switch (self->interlace_pass) {
  case 0:
    x0 = 0;
    y0 = 0;
    dx = 8;
    dy = 8;
    fill_width = 8;
    fill_height = 8;
    break;
  case 1:
    x0 = 4;
    y0 = 0;
    dx = 8;
    dy = 8;
    fill_width = 4;
    fill_height = 8;
    break;
  case 2:
    x0 = 0;
    y0 = 4;
    dx = 4;
    dy = 8;
    fill_width = 4;
    fill_height = 4;
    break;
  case 3:
    x0 = 2;
    y0 = 0;
    dx = 4;
    dy = 4;
    fill_width = 2;
    fill_height = 4;
    break;
  case 4:
    x0 = 0;
    y0 = 2;
    dx = 2;
    dy = 4;
    fill_width = 2;
    fill_height = 2;
    break;
  case 5:
    x0 = 1;
    y0 = 0;
    dx = 2;
    dy = 2;
    fill_width = 1;
    fill_height = 2;
    break;
  case 6:
    x0 = 0;
    y0 = 1;
    dx = 1;
    dy = 2;
    fill_width = 1;
    fill_height = 1;
    break;
  default:
    x0 = 0;
    y0 = 0;
    dx = 1;
    dy = 1;
    fill_width = 1;
    fill_height = 1;
    break;
  }

  // Get functions to read/write samples in the required bit depth.
  size_t n_samples;
  uint8_t (*get_sample)(const uint8_t *, size_t) = NULL;
  void (*write_sample)(uint8_t *, size_t, size_t, size_t, uint8_t) = NULL;
  switch (bit_depth) {
  case 1:
    n_samples = 1;
    get_sample = get_sample1;
    write_sample = write_sample1;
    break;
  case 2:
    n_samples = 1;
    get_sample = get_sample2;
    write_sample = write_sample2;
    break;
  case 4:
    n_samples = 1;
    get_sample = get_sample4;
    write_sample = write_sample4;
    break;
  case 8:
    n_samples = n_channels;
    get_sample = get_sample8;
    write_sample = write_sample8;
    break;
  case 16:
    n_samples = n_channels * 2;
    get_sample = get_sample8;
    write_sample = write_sample8;
    break;
  default:
    return;
  }

  // Write pixels in final image.
  const uint8_t *row_data = ut_uint8_list_get_data(row);
  for (size_t i = 0; i < row_width; i++) {
    // Get area to fill.
    size_t px0 = x0 + (i * dx);
    size_t py0 = y0 + (self->row_count * dy);
    size_t px1 = px0 + fill_width;
    if (px1 > image_width) {
      px1 = image_width;
    }
    size_t py1 = py0 + fill_height;
    if (py1 > image_height) {
      py1 = image_height;
    }

    for (size_t s = 0; s < n_samples; s++) {
      uint8_t sample = get_sample(row_data, i * n_samples + s);
      for (size_t px = px0; px < px1; px++) {
        for (size_t py = py0; py < py1; py++) {
          write_sample(image_data, row_stride, n_samples * px + s, py, sample);
        }
      }
    }
  }
}

static bool is_complete(UtPngDecoder *self) {
  if (self->interlace_method == UT_PNG_INTERLACE_METHOD_ADAM7) {
    return self->interlace_pass >= 7;
  } else {
    return self->row_count >= ut_png_image_get_height(self->image);
  }
}

// Process zlib decoded image data.
static size_t data_decoder_read_cb(UtObject *object, UtObject *data,
                                   bool complete) {
  UtPngDecoder *self = (UtPngDecoder *)object;

  if (ut_object_implements_error(data)) {
    set_error_take(self,
                   ut_cstring_new_printf("Error decoding PNG image data: %s",
                                         ut_error_get_description(data)));
    return 0;
  }

  size_t data_length = ut_list_get_length(data);

  uint32_t image_height = ut_png_image_get_height(self->image);
  uint32_t image_width = ut_png_image_get_width(self->image);
  uint8_t bit_depth = ut_png_image_get_bit_depth(self->image);
  size_t n_channels = ut_png_image_get_n_channels(self->image);

  size_t width, height;
  if (self->interlace_method == UT_PNG_INTERLACE_METHOD_ADAM7) {
    get_adam7_dimensions(image_width, image_height, self->interlace_pass,
                         &width, &height);
  } else {
    width = image_width;
    height = image_height;
  }

  // Read rows.
  size_t offset = 0;
  while (offset < data_length) {
    if (is_complete(self)) {
      set_error(self, "Excess PNG image data");
      return offset;
    }

    // Check sufficient data for row.
    size_t row_stride = (width * bit_depth * n_channels + 7) / 8;
    if (offset + 1 + row_stride > data_length) {
      return offset;
    }

    // Row starts with a filter.
    FilterType filter;
    if (!decode_filter_type(ut_uint8_list_get_element(data, offset), &filter)) {
      set_error(self, "Invalid PNG filter type");
      return offset;
    }
    offset++;

    // Apply filter to row.
    UtObjectRef filtered_row = ut_list_get_sublist(data, offset, row_stride);
    UtObjectRef row = NULL;
    if (self->interlace_method == UT_PNG_INTERLACE_METHOD_ADAM7) {
      // Pass 7 is just every second row, which can be directly written to the
      // final image.
      if (self->interlace_pass == 6) {
        row = ut_list_get_sublist(ut_png_image_get_data(self->image),
                                  (self->row_count * 2 + 1) * row_stride,
                                  row_stride);
      } else {
        // Write to a buffer that will be spread through image.
        row = ut_uint8_array_new_sized(row_stride);
      }
    } else {
      // Write directly to final image.
      row = ut_list_get_sublist(ut_png_image_get_data(self->image),
                                self->row_count * row_stride, row_stride);
    }
    filter_row(self, filter, self->previous_row, filtered_row, row);
    offset += row_stride;

    // Fill pixels from interlaced row.
    if (self->interlace_method == UT_PNG_INTERLACE_METHOD_ADAM7 &&
        self->interlace_pass != 6) {
      apply_adam7_row(self, row, width);
    }

    // Use decoded row as new previous row.
    ut_object_unref(self->previous_row);
    self->previous_row = ut_object_ref(row);
    self->row_count++;

    // Move to next interlace pass, skipping any passes that have no pixels.
    if (self->interlace_method == UT_PNG_INTERLACE_METHOD_ADAM7 &&
        self->row_count >= height) {
      ut_object_unref(self->previous_row);
      self->previous_row = NULL;
      self->row_count = 0;
      do {
        self->interlace_pass++;
        get_adam7_dimensions(image_width, image_height, self->interlace_pass,
                             &width, &height);
      } while (self->interlace_pass < 7 && (width == 0 || height == 0));
    }
  }

  return offset;
}

static void decode_image_header(UtPngDecoder *self, UtObject *data) {
  if (ut_list_get_length(data) != 13) {
    set_error(self, "Insufficient space for PNG image header");
    return;
  }

  uint32_t width = ut_uint8_list_get_uint32_be(data, 0);
  uint32_t height = ut_uint8_list_get_uint32_be(data, 4);
  uint8_t bit_depth = ut_uint8_list_get_element(data, 8);
  uint8_t color_type_value = ut_uint8_list_get_element(data, 9);
  uint8_t compression_method = ut_uint8_list_get_element(data, 10);
  uint8_t filter_method = ut_uint8_list_get_element(data, 11);

  if (width == 0 || height == 0) {
    set_error(self, "Invalid PNG image dimensions");
    return;
  }

  UtPngColorType color_type;
  bool valid_color_type = decode_color_type(color_type_value, &color_type);
  if (!valid_color_type) {
    set_error(self, "Invalid PNG color type");
    return;
  }

  if (!is_valid_bit_depth(color_type, bit_depth)) {
    set_error(self, "Invalid PNG bit depth");
    return;
  }

  bool valid_interlace_method = decode_interlace_method(
      ut_uint8_list_get_element(data, 12), &self->interlace_method);
  if (!valid_interlace_method) {
    set_error(self, "Invalid PNG interlace method");
    return;
  }

  if (compression_method != 0) {
    set_error(self, "Invalid PNG compression method");
    return;
  }

  if (filter_method != 0) {
    set_error(self, "Invalid PNG filter method");
    return;
  }

  size_t n_channels;
  switch (color_type) {
  case UT_PNG_COLOR_TYPE_GREYSCALE:
  case UT_PNG_COLOR_TYPE_INDEXED_COLOR:
    n_channels = 1;
    break;
  case UT_PNG_COLOR_TYPE_GREYSCALE_WITH_ALPHA:
    n_channels = 2;
    break;
  case UT_PNG_COLOR_TYPE_TRUECOLOR:
    n_channels = 3;
    break;
  case UT_PNG_COLOR_TYPE_TRUECOLOR_WITH_ALPHA:
    n_channels = 4;
    break;
  default:
    n_channels = 0;
    break;
  }
  size_t row_stride = (((size_t)width * bit_depth * n_channels) + 7) / 8;
  UtObjectRef image_data = ut_uint8_array_new_sized(height * row_stride);
  UtObjectRef palette = NULL;
  self->image =
      ut_png_image_new(width, height, bit_depth, color_type, image_data);
}

static void decode_palette(UtPngDecoder *self, UtObject *data) {
  size_t palette_length = ut_list_get_length(data);
  if (palette_length % 3 != 0) {
    set_error(self, "Invalid palette size");
    return;
  }

  UtObjectRef palette = ut_uint8_array_new_sized(palette_length);
  uint8_t *palette_data = ut_uint8_list_get_writable_data(palette);
  for (size_t i = 0; i < palette_length; i++) {
    palette_data[i] = ut_uint8_list_get_element(data, i);
  }
  ut_png_image_set_palette(self->image, palette);
}

static void decode_image_data(UtPngDecoder *self, UtObject *data) {
  ut_buffered_input_stream_write(self->image_data_decoder_input_stream, data,
                                 false);
}

static void decode_image_end(UtPngDecoder *self, UtObject *data) {
  if (ut_list_get_length(data) != 0) {
    set_error(self, "Invalid image end PNG chunk");
    return;
  }

  if (!is_complete(self)) {
    set_error(self, "Insufficient PNG image data");
    return;
  }

  set_end(self);
}

static void decode_chromaticities(UtPngDecoder *self, UtObject *data) {}

static void decode_gamma(UtPngDecoder *self, UtObject *data) {}

static void decode_icc_profile(UtPngDecoder *self, UtObject *data) {}

static void decode_significant_bits(UtPngDecoder *self, UtObject *data) {}

static void decode_standard_rgb(UtPngDecoder *self, UtObject *data) {}

static void decode_background(UtPngDecoder *self, UtObject *data) {
  UtObjectRef background = NULL;
  switch (ut_png_image_get_color_type(self->image)) {
  case UT_PNG_COLOR_TYPE_GREYSCALE:
  case UT_PNG_COLOR_TYPE_GREYSCALE_WITH_ALPHA:
    if (ut_list_get_length(data) != 2) {
      set_error(self, "Insufficient space for PNG greyscale background");
      return;
    }
    if (ut_png_image_get_bit_depth(self->image) == 16) {
      background = ut_list_copy(data);
    } else {
      background = ut_uint8_list_new_from_elements(
          1, ut_uint8_list_get_uint16_be(data, 0) & 0xff);
    }
    break;
  case UT_PNG_COLOR_TYPE_TRUECOLOR:
  case UT_PNG_COLOR_TYPE_TRUECOLOR_WITH_ALPHA:
    if (ut_list_get_length(data) != 6) {
      set_error(self, "Insufficient space for PNG truecolor background");
      return;
    }
    if (ut_png_image_get_bit_depth(self->image) == 16) {
      background = ut_list_copy(data);
    } else {
      uint16_t r = ut_uint8_list_get_uint16_be(data, 0);
      uint16_t g = ut_uint8_list_get_uint16_be(data, 2);
      uint16_t b = ut_uint8_list_get_uint16_be(data, 4);
      background =
          ut_uint8_list_new_from_elements(3, r & 0xff, g & 0xff, b & 0xff);
    }
    break;
  case UT_PNG_COLOR_TYPE_INDEXED_COLOR:
    if (ut_list_get_length(data) != 1) {
      set_error(self, "Insufficient space for PNG indexed color background");
      return;
    }
    background = ut_list_copy(data);
    break;
  default:
    assert(false);
  }
  ut_png_image_set_background_color(self->image, background);
}

static void decode_histogram(UtPngDecoder *self, UtObject *data) {}

static void decode_transparency(UtPngDecoder *self, UtObject *data) {}

static void decode_exif(UtPngDecoder *self, UtObject *data) {}

static void decode_physical_dimensions(UtPngDecoder *self, UtObject *data) {
  if (ut_list_get_length(data) != 9) {
    set_error(self, "Insufficient space for PNG image dimensions");
    return;
  }

  /*self->pixels_per_unit_x =*/ut_uint8_list_get_uint32_be(data, 0);
  /*self->pixels_per_unix_y =*/ut_uint8_list_get_uint32_be(data, 4);
  /*self->unit_specifier =*/ut_uint8_list_get_element(data, 8);
}

static void decode_suggested_palette(UtPngDecoder *self, UtObject *data) {}

static void decode_modification_time(UtPngDecoder *self, UtObject *data) {
  if (ut_list_get_length(data) != 7) {
    set_error(self, "Insufficient space for PNG modification time");
    return;
  }

  /*self->year =*/ut_uint8_list_get_uint16_be(data, 0);
  /*self->month =*/ut_uint8_list_get_element(data, 2);
  /*self->day =*/ut_uint8_list_get_element(data, 3);
  /*self->hour =*/ut_uint8_list_get_element(data, 4);
  /*self->minute =*/ut_uint8_list_get_element(data, 5);
  /*self->second =*/ut_uint8_list_get_element(data, 6);
}

static size_t find_separator(UtObject *data, size_t start) {
  size_t data_length = ut_list_get_length(data);
  for (size_t i = start; i < data_length; i++) {
    if (ut_uint8_list_get_element(data, i) == '\0') {
      return i;
    }
  }

  return data_length;
}

static bool is_printable(uint8_t character) {
  return (character >= 32 && character <= 126) ||
         (character >= 161 && character <= 255);
}

static bool is_alphanumeric(uint8_t character) {
  return (character >= 'a' && character <= 'z') ||
         (character >= 'A' && character <= 'Z') ||
         (character >= '0' && character <= '9');
}

static bool valid_keyword(UtObject *data) {
  size_t data_length = ut_list_get_length(data);
  if (data_length > 79) {
    return false;
  }

  for (size_t i = 0; i < data_length; i++) {
    uint8_t c = ut_uint8_list_get_element(data, i);
    if (!is_printable(c)) {
      return false;
    }
  }

  return true;
}

static bool valid_language(UtObject *data) {
  size_t data_length = ut_list_get_length(data);
  for (size_t i = 0; i < data_length; i++) {
    uint8_t c = ut_uint8_list_get_element(data, i);
    if (!is_alphanumeric(c) && c != '-') {
      return false;
    }
  }

  return true;
}

static bool valid_text(UtObject *data) {
  size_t data_length = ut_list_get_length(data);
  for (size_t i = 0; i < data_length; i++) {
    uint8_t c = ut_uint8_list_get_element(data, i);
    if (!is_printable(c) && c != '\n') {
      return false;
    }
  }

  return true;
}

static UtObject *decompress_text(UtPngDecoder *self, uint8_t compression_method,
                                 UtObject *compressed_text) {
  if (compression_method != 0) {
    set_error(self, "Invalid compressed text compression method");
    return NULL;
  }
  UtObjectRef zlib_input = ut_list_input_stream_new(compressed_text);
  UtObjectRef decoder = ut_zlib_decoder_new(zlib_input);
  UtObjectRef text = ut_input_stream_read_sync(decoder);
  if (ut_object_implements_error(text)) {
    set_error(self, "Invalid compressed text");
    return NULL;
  }
  return ut_object_ref(text);
}

static void decode_international_text(UtPngDecoder *self, UtObject *data) {
  size_t data_length = ut_list_get_length(data);
  size_t keyword_end = find_separator(data, 0);
  if (keyword_end >= data_length) {
    set_error(self, "Missing separator in PNG international text block");
    return;
  }

  if (keyword_end + 3 >= data_length) {
    set_error(self, "Invalid PNG international text block");
    return;
  }

  uint8_t compression_flag = ut_uint8_list_get_element(data, keyword_end + 1);
  uint8_t compression_method = ut_uint8_list_get_element(data, keyword_end + 2);
  if (compression_flag > 1) {
    set_error(self, "Invalid PNG international text block compression flag");
  }

  size_t language_start = keyword_end + 3;
  size_t language_end = find_separator(data, language_start);
  if (language_end >= data_length) {
    set_error(self,
              "Missing language separator in PNG international text block");
    return;
  }

  size_t translated_keyword_start = language_end + 1;
  size_t translated_keyword_end = find_separator(data, language_end + 3);
  if (translated_keyword_end >= data_length) {
    set_error(
        self,
        "Missing translated keyword separator in PNG international text block");
    return;
  }

  size_t text_start = translated_keyword_end + 1;

  UtObjectRef keyword = ut_list_get_sublist(data, 0, keyword_end);
  UtObjectRef language =
      ut_list_get_sublist(data, language_start, language_end - language_start);
  UtObjectRef translated_keyword =
      ut_list_get_sublist(data, translated_keyword_start,
                          translated_keyword_end - translated_keyword_start);
  UtObjectRef text_data =
      ut_list_get_sublist(data, text_start, data_length - text_start);
  UtObjectRef text = NULL;
  if (compression_flag != 0) {
    text = decompress_text(self, compression_method, text);
    if (text == NULL) {
      return;
    }
  } else {
    text = ut_object_ref(text_data);
  }

  if (!valid_keyword(keyword)) {
    set_error(self, "International text keyword contains invalid characters");
    return;
  }
  if (!valid_language(language)) {
    set_error(self, "International text language contains invalid characters");
    return;
  }

  UtObjectRef keyword_string = ut_string_new_from_iso_8859_1(keyword);
  UtObjectRef language_string = ut_string_new_from_iso_8859_1(language);
  UtObjectRef translated_keyword_string =
      ut_string_new_from_utf8(translated_keyword);
  UtObjectRef text_string = ut_string_new_from_utf8(text);

  ut_png_image_set_international_text(
      self->image, ut_string_get_text(keyword_string),
      ut_string_get_text(language_string),
      ut_string_get_text(translated_keyword_string),
      ut_string_get_text(text_string));
}

static void decode_text(UtPngDecoder *self, UtObject *data) {
  size_t data_length = ut_list_get_length(data);
  size_t keyword_end = find_separator(data, 0);
  if (keyword_end >= data_length) {
    set_error(self, "Missing separator in text block");
    return;
  }

  UtObjectRef keyword = ut_list_get_sublist(data, 0, keyword_end);
  size_t text_start = keyword_end + 1;
  UtObjectRef text =
      ut_list_get_sublist(data, text_start, data_length - text_start);

  if (!valid_keyword(keyword)) {
    set_error(self, "Text keyword contains invalid characters");
    return;
  }
  if (!valid_text(text)) {
    set_error(self, "Text contains invalid characters");
    return;
  }

  UtObjectRef keyword_string = ut_string_new_from_iso_8859_1(keyword);
  UtObjectRef text_string = ut_string_new_from_iso_8859_1(text);

  ut_png_image_set_text(self->image, ut_string_get_text(keyword_string),
                        ut_string_get_text(text_string));
}

static void decode_compressed_text(UtPngDecoder *self, UtObject *data) {
  size_t data_length = ut_list_get_length(data);
  size_t keyword_end = find_separator(data, 0);
  if (keyword_end >= data_length) {
    set_error(self, "Missing separator in compressed text block");
    return;
  }

  if (keyword_end + 1 >= data_length) {
    set_error(self, "Invalid PNG compressed text block");
    return;
  }

  UtObjectRef keyword = ut_list_get_sublist(data, 0, keyword_end);
  uint8_t compression_method = ut_uint8_list_get_element(data, keyword_end + 1);
  size_t compressed_text_start = keyword_end + 2;
  UtObjectRef compressed_text = ut_list_get_sublist(
      data, compressed_text_start, data_length - compressed_text_start);

  if (!valid_keyword(keyword)) {
    set_error(self, "Text keyword contains invalid characters");
    return;
  }
  UtObjectRef text = decompress_text(self, compression_method, compressed_text);
  if (text == NULL) {
    return;
  }
  if (!valid_text(text)) {
    set_error(self, "Text contains invalid characters");
    return;
  }

  UtObjectRef keyword_string = ut_string_new_from_iso_8859_1(keyword);
  UtObjectRef text_string = ut_string_new_from_iso_8859_1(text);

  ut_png_image_set_text(self->image, ut_string_get_text(keyword_string),
                        ut_string_get_text(text_string));
}

static size_t decode_chunk(UtPngDecoder *self, UtObject *data) {
  size_t data_length = ut_list_get_length(data);

  // Minimum chunk is 12 bytes (length + type + CRC).
  if (data_length < 12) {
    return 0;
  }

  size_t offset = 0;
  uint32_t chunk_data_length = ut_uint8_list_get_uint32_be(data, offset);
  offset += 4;
  size_t total_chunk_length = 12 + chunk_data_length;
  if (data_length < total_chunk_length) {
    return 0;
  }

  UtPngChunkType type = ut_uint8_list_get_uint32_be(data, offset);
  offset += 4;
  bool is_ancillary = (type & 0x20000000) != 0;

  UtObjectRef chunk_data = ut_list_get_sublist(data, offset, chunk_data_length);
  offset += chunk_data_length;
  uint32_t crc = ut_uint8_list_get_uint32_be(data, offset);
  offset += 4;
  uint32_t calculated_crc = crc32(data, 4, 4 + chunk_data_length);
  if (calculated_crc != crc) {
    set_error(self, "PNG chunk CRC mismatch");
    return 0;
  }

  switch (type) {
  case UT_PNG_CHUNK_TYPE_IMAGE_HEADER:
    decode_image_header(self, chunk_data);
    break;
  case UT_PNG_CHUNK_TYPE_PALETTE:
    decode_palette(self, chunk_data);
    break;
  case UT_PNG_CHUNK_TYPE_IMAGE_DATA:
    decode_image_data(self, chunk_data);
    break;
  case UT_PNG_CHUNK_TYPE_IMAGE_END:
    decode_image_end(self, chunk_data);
    break;
  case UT_PNG_CHUNK_TYPE_CHROMATICITIES:
    decode_chromaticities(self, chunk_data);
    break;
  case UT_PNG_CHUNK_TYPE_GAMMA:
    decode_gamma(self, chunk_data);
    break;
  case UT_PNG_CHUNK_TYPE_ICC_PROFILE:
    decode_icc_profile(self, chunk_data);
    break;
  case UT_PNG_CHUNK_TYPE_SIGNIFICANT_BITS:
    decode_significant_bits(self, chunk_data);
    break;
  case UT_PNG_CHUNK_TYPE_STANDARD_RGB:
    decode_standard_rgb(self, chunk_data);
    break;
  case UT_PNG_CHUNK_TYPE_BACKGROUND:
    decode_background(self, chunk_data);
    break;
  case UT_PNG_CHUNK_TYPE_HISTOGRAM:
    decode_histogram(self, chunk_data);
    break;
  case UT_PNG_CHUNK_TYPE_TRANSPARENCY:
    decode_transparency(self, chunk_data);
    break;
  case UT_PNG_CHUNK_TYPE_EXIF:
    decode_exif(self, chunk_data);
    break;
  case UT_PNG_CHUNK_TYPE_PHYSICAL_DIMENSIONS:
    decode_physical_dimensions(self, chunk_data);
    break;
  case UT_PNG_CHUNK_TYPE_SUGGESTED_PALETTE:
    decode_suggested_palette(self, chunk_data);
    break;
  case UT_PNG_CHUNK_TYPE_MODIFICATION_TIME:
    decode_modification_time(self, chunk_data);
    break;
  case UT_PNG_CHUNK_TYPE_INTERNATIONAL_TEXT:
    decode_international_text(self, chunk_data);
    break;
  case UT_PNG_CHUNK_TYPE_TEXT:
    decode_text(self, chunk_data);
    break;
  case UT_PNG_CHUNK_TYPE_COMPRESSED_TEXT:
    decode_compressed_text(self, chunk_data);
    break;
  default:
    if (!is_ancillary) {
      set_error(self, "Uknown critical PNG chunk");
      return 0;
    }
    break;
  }

  return total_chunk_length;
}

static size_t read_cb(UtObject *object, UtObject *data, bool complete) {
  UtPngDecoder *self = (UtPngDecoder *)object;

  if (ut_object_implements_error(data)) {
    set_error_take(self, ut_cstring_new_printf("Failed to read PNG data: %s",
                                               ut_error_get_description(data)));
    return 0;
  }

  size_t data_length = ut_list_get_length(data);
  size_t offset = 0;
  while (true) {
    size_t n_used;
    UtObjectRef d = ut_list_get_sublist(data, offset, data_length - offset);
    DecoderState old_state = self->state;
    switch (self->state) {
    case DECODER_STATE_SIGNATURE:
      n_used = decode_signature(self, d);
      break;
    case DECODER_STATE_CHUNK:
      n_used = decode_chunk(self, d);
      break;
    case DECODER_STATE_ERROR:
    case DECODER_STATE_END:
      return offset;
    default:
      assert(false);
    }

    if (self->state == old_state && n_used == 0) {
      if (complete && self->state != DECODER_STATE_END) {
        set_error(self, "Incomplete PNG");
      }
      return offset;
    }

    offset += n_used;
  }
}

static void done_cb(UtObject *object) {}

static void ut_png_decoder_init(UtObject *object) {
  UtPngDecoder *self = (UtPngDecoder *)object;
  self->state = DECODER_STATE_SIGNATURE;
}

static void ut_png_decoder_cleanup(UtObject *object) {
  UtPngDecoder *self = (UtPngDecoder *)object;

  ut_input_stream_close(self->input_stream);
  ut_input_stream_close(self->image_data_decoder);

  ut_object_unref(self->input_stream);
  ut_object_weak_unref(&self->callback_object);
  ut_object_unref(self->image_data_decoder_input_stream);
  ut_object_unref(self->image_data_decoder);
  ut_object_unref(self->previous_row);
  ut_object_unref(self->image);
  ut_object_unref(self->error);
}

static UtObjectInterface object_interface = {.type_name = "UtPngDecoder",
                                             .init = ut_png_decoder_init,
                                             .cleanup = ut_png_decoder_cleanup};

UtObject *ut_png_decoder_new(UtObject *input_stream) {
  UtObject *object = ut_object_new(sizeof(UtPngDecoder), &object_interface);
  UtPngDecoder *self = (UtPngDecoder *)object;
  self->input_stream = ut_object_ref(input_stream);
  return object;
}

void ut_png_decoder_decode(UtObject *object, UtObject *callback_object,
                           UtPngDecodeCallback callback) {
  assert(ut_object_is_png_decoder(object));
  UtPngDecoder *self = (UtPngDecoder *)object;

  assert(self->callback == NULL);
  assert(callback != NULL);

  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callback = callback;

  self->image_data_decoder_input_stream = ut_buffered_input_stream_new();
  self->image_data_decoder =
      ut_zlib_decoder_new(self->image_data_decoder_input_stream);
  ut_input_stream_read(self->image_data_decoder, object, data_decoder_read_cb);

  ut_input_stream_read(self->input_stream, object, read_cb);
}

UtObject *ut_png_decoder_decode_sync(UtObject *object) {
  assert(ut_object_is_png_decoder(object));
  UtPngDecoder *self = (UtPngDecoder *)object;

  UtObjectRef dummy_object = ut_null_new();
  ut_png_decoder_decode(object, dummy_object, done_cb);
  if (self->error != NULL) {
    return ut_object_ref(self->error);
  }

  return ut_object_ref(self->image);
}

UtObject *ut_png_decoder_get_error(UtObject *object) {
  assert(ut_object_is_png_decoder(object));
  UtPngDecoder *self = (UtPngDecoder *)object;
  return self->error;
}

UtObject *ut_png_decoder_get_image(UtObject *object) {
  assert(ut_object_is_png_decoder(object));
  UtPngDecoder *self = (UtPngDecoder *)object;
  return self->image;
}

bool ut_object_is_png_decoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
