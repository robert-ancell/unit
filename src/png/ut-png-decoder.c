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
  UtObject *input_stream;
  UtObject *read_cancel;

  UtPngDecodeCallback callback;
  void *user_data;
  UtObject *cancel;

  DecoderState state;
  UtObject *error;
  UtObject *prev_line;
  UtObject *line;
  FilterType line_filter;
  size_t line_count;

  uint32_t width;
  uint32_t height;
  uint8_t bit_depth;
  UtPngColourType colour_type;
  UtPngInterlaceMethod interlace_method;
  uint8_t n_channels;
  size_t rowstride;
  UtObject *data;
  UtObject *image;
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

static void set_error(UtPngDecoder *self, const char *description) {
  if (self->state == DECODER_STATE_ERROR) {
    return;
  }

  self->error = ut_png_error_new(description);
  self->state = DECODER_STATE_ERROR;
}

static size_t decode_signature(UtPngDecoder *self, UtObject *data,
                               size_t offset) {
  if (ut_list_get_length(data) - offset < 8) {
    return 0;
  }

  if (ut_uint8_list_get_element(data, offset + 0) != 137 ||
      ut_uint8_list_get_element(data, offset + 1) != 80 ||
      ut_uint8_list_get_element(data, offset + 2) != 78 ||
      ut_uint8_list_get_element(data, offset + 3) != 71 ||
      ut_uint8_list_get_element(data, offset + 4) != 13 ||
      ut_uint8_list_get_element(data, offset + 5) != 10 ||
      ut_uint8_list_get_element(data, offset + 6) != 26 ||
      ut_uint8_list_get_element(data, offset + 7) != 10) {
    set_error(self, "Invalid PNG signature");
    return 0;
  }

  self->state = DECODER_STATE_CHUNK;
  return 8;
}

static bool decode_colour_type(uint8_t value, UtPngColourType *type,
                               uint8_t *n_channels) {
  switch (value) {
  case 0:
    *type = UT_PNG_COLOUR_TYPE_GREYSCALE;
    *n_channels = 1;
    return true;
  case 2:
    *type = UT_PNG_COLOUR_TYPE_TRUECOLOUR;
    *n_channels = 3;
    return true;
  case 3:
    *type = UT_PNG_COLOUR_TYPE_INDEXED_COLOUR;
    *n_channels = 1;
    return true;
  case 4:
    *type = UT_PNG_COLOUR_TYPE_GREYSCALE_WITH_ALPHA;
    *n_channels = 2;
    return true;
  case 6:
    *type = UT_PNG_COLOUR_TYPE_TRUECOLOUR_WITH_ALPHA;
    *n_channels = 4;
    return true;
  default:
    return false;
  }
}

static bool is_valid_bit_depth(uint8_t type, uint8_t depth) {
  switch (type) {
  case UT_PNG_COLOUR_TYPE_GREYSCALE:
    return depth == 1 || depth == 2 || depth == 4 || depth == 8 || depth == 16;
  case UT_PNG_COLOUR_TYPE_TRUECOLOUR:
    return depth == 8 || depth == 16;
  case UT_PNG_COLOUR_TYPE_INDEXED_COLOUR:
    return depth == 1 || depth == 2 || depth == 4 || depth == 8;
  case UT_PNG_COLOUR_TYPE_GREYSCALE_WITH_ALPHA:
    return depth == 8 || depth == 16;
  case UT_PNG_COLOUR_TYPE_TRUECOLOUR_WITH_ALPHA:
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

static void decode_image_header(UtPngDecoder *self, UtObject *data) {
  if (ut_list_get_length(data) != 13) {
    set_error(self, "Insufficient space for PNG image header");
    return;
  }

  self->width = ut_uint8_list_get_uint32_be(data, 0);
  self->height = ut_uint8_list_get_uint32_be(data, 4);
  self->bit_depth = ut_uint8_list_get_element(data, 8);
  uint8_t colour_type = ut_uint8_list_get_element(data, 9);
  uint8_t compression_method = ut_uint8_list_get_element(data, 10);
  uint8_t filter_method = ut_uint8_list_get_element(data, 11);

  if (self->width == 0 || self->height == 0) {
    set_error(self, "Invalid PNG image dimensions");
    return;
  }

  bool valid_colour_type =
      decode_colour_type(colour_type, &self->colour_type, &self->n_channels);
  if (!valid_colour_type) {
    set_error(self, "Invalid PNG colour type");
    return;
  }

  if (!is_valid_bit_depth(self->colour_type, self->bit_depth)) {
    set_error(self, "Invalid PNG bit depth");
    return;
  }

  bool valid_interlace_method = decode_interlace_method(
      ut_uint8_list_get_element(data, 12), &self->interlace_method);
  if (!valid_interlace_method) {
    set_error(self, "Invalid PNG interlace method");
    return;
  }

  self->rowstride =
      (((size_t)self->width * self->bit_depth * self->n_channels) + 7) / 8;

  if (compression_method != 0) {
    set_error(self, "Invalid PNG compression method");
    return;
  }

  if (filter_method != 0) {
    set_error(self, "Invalid PNG filter method");
    return;
  }
  ut_object_unref(self->prev_line);
  self->prev_line = ut_uint8_array_new_sized(self->rowstride);
  ut_object_unref(self->line);
  self->line = NULL;
  self->line_count = 0;

  self->data = ut_uint8_array_new_sized(self->height * self->rowstride);
  self->image = ut_png_image_new(self->width, self->height, self->bit_depth,
                                 self->colour_type, self->data);
}

static void decode_palette(UtPngDecoder *self, UtObject *data) {}

static void process_image_data(UtPngDecoder *self, UtObject *data) {
  size_t data_length = ut_list_get_length(data);

  uint8_t *image_data = ut_uint8_array_get_data(self->data);

  size_t offset = 0;
  while (offset < data_length) {
    if (self->line == NULL) {
      if (self->line_count == self->height) {
        set_error(self, "Excess image data");
        return;
      }

      if (!decode_filter_type(ut_uint8_list_get_element(data, offset),
                              &self->line_filter)) {
        set_error(self, "Invalid PNG filter type");
        return;
      }
      offset++;
      self->line = ut_uint8_array_new(); // FIXME: sized
    }

    ut_uint8_list_append(self->line, ut_uint8_list_get_element(data, offset));
    offset++;

    if (ut_list_get_length(self->line) == self->rowstride) {
      size_t pixel_width = (self->bit_depth * self->n_channels) / 8;
      if (pixel_width == 0) {
        pixel_width = 1;
      }

      size_t image_data_offset = self->line_count * self->rowstride;
      switch (self->line_filter) {
      case FILTER_TYPE_NONE:
        for (size_t i = 0; i < self->rowstride; i++) {
          image_data[image_data_offset] =
              ut_uint8_list_get_element(self->line, i);
          image_data_offset++;
        }
        break;
      case FILTER_TYPE_SUB:
        // FIXME: optimised by having filter functions for each image
        // depth/format
        for (size_t i = 0; i < pixel_width; i++) {
          image_data[image_data_offset] =
              ut_uint8_list_get_element(self->line, i);
          image_data_offset++;
        }
        for (size_t i = pixel_width; i < self->rowstride; i += pixel_width) {
          for (size_t j = 0; j < pixel_width; j++) {
            uint8_t x = ut_uint8_list_get_element(self->line, i + j);
            uint8_t a =
                ut_uint8_list_get_element(self->line, i + j - pixel_width);
            image_data[image_data_offset] = x + a;
            image_data_offset++;
          }
        }
        break;
      case FILTER_TYPE_UP:
        for (size_t i = 0; i < self->rowstride; i++) {
          uint8_t x = ut_uint8_list_get_element(self->line, i);
          uint8_t b = ut_uint8_list_get_element(self->prev_line, i);
          image_data[image_data_offset] = x + b;
          image_data_offset++;
        }
        break;
      case FILTER_TYPE_AVERAGE:
        for (size_t i = 0; i < pixel_width; i++) {
          image_data[image_data_offset] =
              ut_uint8_list_get_element(self->line, i);
          image_data_offset++;
        }
        for (size_t i = pixel_width; i < self->rowstride; i += pixel_width) {
          for (size_t j = 0; i < pixel_width; j++) {
            uint16_t a =
                ut_uint8_list_get_element(self->line, i + j - pixel_width);
            uint16_t b = ut_uint8_list_get_element(self->prev_line, i + j);
            image_data[image_data_offset] = (a + b) / 2;
            image_data_offset++;
          }
        }
        break;
      case FILTER_TYPE_PAETH:
        for (size_t i = 0; i < pixel_width; i++) {
          image_data[image_data_offset] =
              ut_uint8_list_get_element(self->line, i);
          image_data_offset++;
        }
        for (size_t i = pixel_width; i < self->rowstride; i += pixel_width) {
          for (size_t j = 0; i < pixel_width; j++) {
            uint8_t a =
                ut_uint8_list_get_element(self->line, i + j - pixel_width);
            uint8_t b = ut_uint8_list_get_element(self->prev_line, i + j);
            uint8_t c =
                ut_uint8_list_get_element(self->prev_line, i + j - pixel_width);
            uint8_t p = a + b - c;
            uint8_t pa = p > a ? p - a : a - p;
            uint8_t pb = p > b ? p - b : b - p;
            uint8_t pc = p > c ? p - c : c - p;
            uint8_t value;
            if (pa <= pb && pa <= pc) {
              value = a;
            } else if (pb <= pc) {
              value = b;
            } else {
              value = c;
            }
            image_data[image_data_offset] = value;
            image_data_offset++;
          }
        }
        break;
      }

      ut_object_unref(self->prev_line);
      self->prev_line = self->line;
      self->line = NULL;
      self->line_count++;
    }
  }
}

static void decode_image_data(UtPngDecoder *self, UtObject *data) {
  UtObjectRef zlib_stream = ut_list_input_stream_new(data);
  UtObjectRef zlib_decoder = ut_zlib_decoder_new(zlib_stream);
  UtObjectRef image_data = ut_input_stream_read_sync(zlib_decoder);
  if (ut_object_implements_error(image_data)) {
    ut_cstring_ref description =
        ut_cstring_new_printf("Error decoding PNG image data: %s",
                              ut_error_get_description(image_data));
    set_error(self, description);
    return;
  }
  process_image_data(self, image_data);
}

static void decode_image_end(UtPngDecoder *self, UtObject *data) {
  if (ut_list_get_length(data) != 0) {
    set_error(self, "Extra data after PNG image");
    return;
  }

  self->state = DECODER_STATE_END;
}

static void decode_background(UtPngDecoder *self, UtObject *data) {
  switch (self->colour_type) {
  case UT_PNG_COLOUR_TYPE_GREYSCALE:
  case UT_PNG_COLOUR_TYPE_GREYSCALE_WITH_ALPHA:
    if (ut_list_get_length(data) != 2) {
      self->error =
          ut_png_error_new("Insufficient space for PNG greyscale background");
      return;
    }
    /*self->background_colour = */ ut_uint8_list_get_uint16_be(data, 0);
    break;
  case UT_PNG_COLOUR_TYPE_TRUECOLOUR:
  case UT_PNG_COLOUR_TYPE_TRUECOLOUR_WITH_ALPHA:
    if (ut_list_get_length(data) != 6) {
      set_error(self, "Insufficient space for PNG truecolour background");
      /*self->background_colour_r = */ ut_uint8_list_get_uint16_be(data, 0);
      /*self->background_colour_g = */ ut_uint8_list_get_uint16_be(data, 2);
      /*self->background_colour_b = */ ut_uint8_list_get_uint16_be(data, 4);
      return;
    }
    break;
  case UT_PNG_COLOUR_TYPE_INDEXED_COLOUR:
    if (ut_list_get_length(data) != 1) {
      set_error(self, "Insufficient space for PNG indexed colour background");
      return;
    }
    /*self->background_colour_b = */ ut_uint8_list_get_element(data, 0);
    break;
  default:
    assert(false);
  }
}

static void decode_physical_dimensions(UtPngDecoder *self, UtObject *data) {
  if (ut_list_get_length(data) != 9) {
    set_error(self, "Insufficient space for PNG image dimensions");
    return;
  }

  /*self->pixels_per_unit_x =*/ut_uint8_list_get_uint32_be(data, 0);
  /*self->pixels_per_unix_y =*/ut_uint8_list_get_uint32_be(data, 4);
  /*self->unit_specifier =*/ut_uint8_list_get_element(data, 8);
}

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

static size_t decode_chunk(UtPngDecoder *self, UtObject *data, size_t offset) {
  size_t data_length = ut_list_get_length(data) - offset;

  // Minimum chunk is 12 bytes (length + type + CRC).
  if (data_length < 12) {
    return 0;
  }

  uint32_t chunk_data_length = ut_uint8_list_get_uint32_be(data, offset + 0);
  size_t total_chunk_length = 12 + chunk_data_length;
  if (data_length < total_chunk_length) {
    return 0;
  }

  UtPngChunkType type = ut_uint8_list_get_uint32_be(data, offset + 4);

  size_t chunk_data_offset = offset + 8;
  UtObjectRef chunk_data =
      ut_list_get_sublist(data, chunk_data_offset, chunk_data_length);
  uint32_t crc =
      ut_uint8_list_get_uint32_be(data, chunk_data_offset + chunk_data_length);
  uint32_t calculated_crc = crc32(data, offset + 4, 4 + chunk_data_length);
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
  case UT_PNG_CHUNK_TYPE_CHROMATICITIES:
    break;
  case UT_PNG_CHUNK_TYPE_GAMMA:
    break;
  case UT_PNG_CHUNK_TYPE_ICC_PROFILE:
    break;
  case UT_PNG_CHUNK_TYPE_SIGNIFICANT_BITS:
    break;
  case UT_PNG_CHUNK_TYPE_STANDARD_RGB:
    break;
  case UT_PNG_CHUNK_TYPE_IMAGE_END:
    decode_image_end(self, chunk_data);
    break;
  case UT_PNG_CHUNK_TYPE_BACKGROUND:
    decode_background(self, chunk_data);
    break;
  case UT_PNG_CHUNK_TYPE_HISTOGRAM:
    break;
  case UT_PNG_CHUNK_TYPE_PHYSICAL_DIMENSIONS:
    decode_physical_dimensions(self, chunk_data);
    break;
  case UT_PNG_CHUNK_TYPE_SUGGESTED_PALETTE:
    break;
  case UT_PNG_CHUNK_TYPE_MODIFICATION_TIME:
    decode_modification_time(self, chunk_data);
    break;
  case UT_PNG_CHUNK_TYPE_TEXT:
    break;
  case UT_PNG_CHUNK_TYPE_COMPRESSED_TEXT:
    break;
  case UT_PNG_CHUNK_TYPE_INTERNATIONAL_TEXT:
    break;
  }

  return total_chunk_length;
}

static size_t read_cb(void *user_data, UtObject *data, bool complete) {
  UtPngDecoder *self = user_data;

  if (ut_cancel_is_active(self->cancel)) {
    ut_cancel_activate(self->read_cancel);
    return 0;
  }

  size_t offset = 0;
  while (true) {
    size_t n_used;
    switch (self->state) {
    case DECODER_STATE_SIGNATURE:
      n_used = decode_signature(self, data, offset);
      break;
    case DECODER_STATE_CHUNK:
      n_used = decode_chunk(self, data, offset);
      break;
    case DECODER_STATE_ERROR:
      ut_cancel_activate(self->read_cancel);
      self->callback(self->user_data);
      return offset;
    case DECODER_STATE_END:
      ut_cancel_activate(self->read_cancel);
      self->callback(self->user_data);
      return offset;
    default:
      assert(false);
    }

    if (n_used == 0 && self->state != DECODER_STATE_ERROR) {
      return offset;
    }

    offset += n_used;
  }
}

static void done_cb(void *user_data) {}

static void ut_png_decoder_init(UtObject *object) {
  UtPngDecoder *self = (UtPngDecoder *)object;
  self->read_cancel = ut_cancel_new();
  self->state = DECODER_STATE_SIGNATURE;
  self->line_filter = FILTER_TYPE_NONE;
}

static void ut_png_decoder_cleanup(UtObject *object) {
  UtPngDecoder *self = (UtPngDecoder *)object;
  ut_cancel_activate(self->read_cancel);
  ut_object_unref(self->input_stream);
  ut_object_unref(self->read_cancel);
  ut_object_unref(self->cancel);
  ut_object_unref(self->error);
  ut_object_unref(self->prev_line);
  ut_object_unref(self->line);
  ut_object_unref(self->data);
}

static UtObjectInterface object_interface = {.type_name = "UtPngDecoder",
                                             .init = ut_png_decoder_init,
                                             .cleanup = ut_png_decoder_cleanup,
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_png_decoder_new(UtObject *input_stream) {
  UtObject *object = ut_object_new(sizeof(UtPngDecoder), &object_interface);
  UtPngDecoder *self = (UtPngDecoder *)object;
  self->input_stream = ut_object_ref(input_stream);
  return object;
}

void ut_png_decoder_decode(UtObject *object, UtPngDecodeCallback callback,
                           void *user_data, UtObject *cancel) {
  assert(ut_object_is_png_decoder(object));
  UtPngDecoder *self = (UtPngDecoder *)object;

  assert(self->callback == NULL);
  assert(callback != NULL);

  self->callback = callback;
  self->user_data = user_data;
  self->cancel = ut_object_ref(cancel);

  ut_input_stream_read(self->input_stream, read_cb, self, self->read_cancel);
}

UtObject *ut_png_decoder_decode_sync(UtObject *object) {
  assert(ut_object_is_png_decoder(object));
  UtPngDecoder *self = (UtPngDecoder *)object;

  ut_png_decoder_decode(object, done_cb, NULL, NULL);
  if (self->state != DECODER_STATE_END) {
    if (self->error != NULL) {
      return ut_object_ref(self->error);
    } else {
      return ut_general_error_new("Sync call did not complete");
    }
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
