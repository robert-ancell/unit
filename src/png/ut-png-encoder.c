#include <assert.h>

#include "ut-png.h"
#include "ut.h"

#define COMPRESS_DEFLATE 0

typedef struct {
  UtObject object;
  UtObject *image;

  UtObject *output_stream;
} UtPngEncoder;

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

static uint8_t encode_color_type(UtPngColorType type) {
  switch (type) {
  case UT_PNG_COLOR_TYPE_GREYSCALE:
    return 0;
  case UT_PNG_COLOR_TYPE_TRUECOLOR:
    return 2;
  case UT_PNG_COLOR_TYPE_INDEXED_COLOR:
    return 3;
  case UT_PNG_COLOR_TYPE_GREYSCALE_WITH_ALPHA:
    return 4;
  case UT_PNG_COLOR_TYPE_TRUECOLOR_WITH_ALPHA:
    return 6;
  default:
    return 0;
  }
}

static uint8_t encode_interlace_method(UtPngInterlaceMethod method) {
  switch (method) {
  case UT_PNG_INTERLACE_METHOD_NONE:
    return 0;
  case UT_PNG_INTERLACE_METHOD_ADAM7:
    return 1;
  default:
    return 0xff;
  }
}

static void write_signature(UtPngEncoder *self) {
  UtObjectRef signature =
      ut_uint8_list_new_from_elements(8, 137, 80, 78, 71, 13, 10, 26, 10);
  ut_output_stream_write(self->output_stream, signature);
}

static void write_chunk(UtPngEncoder *self, UtObject *chunk) {
  size_t chunk_length = ut_list_get_length(chunk);
  assert(chunk_length >= 8 && chunk_length <= 8 + (size_t)0xffffffff);

  // Overwrite length field.
  size_t length = chunk_length - 8;
  uint8_t *d = ut_uint8_list_get_writable_data(chunk);
  d[0] = length >> 24;
  d[1] = (length >> 16) & 0xff;
  d[2] = (length >> 8) & 0xff;
  d[3] = length & 0xff;

  // Append CRC.
  ut_uint8_list_append_uint32_be(chunk, crc32(chunk, 4, chunk_length - 4));

  ut_output_stream_write(self->output_stream, chunk);
}

static UtObject *start_chunk(uint32_t type) {
  UtObject *chunk = ut_uint8_list_new();
  // Leave space for length to be filled in in write_chunk().
  ut_uint8_list_append_uint32_be(chunk, 0);
  ut_uint8_list_append_uint32_be(chunk, type);
  return chunk;
}

static void write_image_header(UtPngEncoder *self, uint32_t width,
                               uint32_t height, uint8_t bit_depth,
                               uint8_t color_type, uint8_t compression_method,
                               uint8_t filter_method,
                               uint8_t interlace_method) {
  UtObjectRef chunk = start_chunk(UT_PNG_CHUNK_TYPE_IMAGE_HEADER);
  ut_uint8_list_append_uint32_be(chunk, width);
  ut_uint8_list_append_uint32_be(chunk, height);
  ut_uint8_list_append(chunk, bit_depth);
  ut_uint8_list_append(chunk, color_type);
  ut_uint8_list_append(chunk, compression_method);
  ut_uint8_list_append(chunk, filter_method);
  ut_uint8_list_append(chunk, interlace_method);
  write_chunk(self, chunk);
}

static void write_palette(UtPngEncoder *self, UtObject *palette) {
  UtObjectRef chunk = start_chunk(UT_PNG_CHUNK_TYPE_PALETTE);
  ut_list_append_list(chunk, palette);
  write_chunk(self, chunk);
}

static void write_background(UtPngEncoder *self, UtObject *background_color) {
  UtObjectRef chunk = start_chunk(UT_PNG_CHUNK_TYPE_BACKGROUND);
  ut_list_append_list(chunk, background_color);
  write_chunk(self, chunk);
}

static size_t zlib_data_cb(void *user_data, UtObject *data, bool complete) {
  UtObject *chunk = user_data;
  ut_list_append_list(chunk, data);
  return ut_list_get_length(data);
}

static void write_image_data(UtPngEncoder *self) {
  UtObjectRef chunk = start_chunk(UT_PNG_CHUNK_TYPE_IMAGE_DATA);

  size_t image_height = ut_png_image_get_height(self->image);
  size_t row_stride = ut_png_image_get_row_stride(self->image);
  UtObject *image_data = ut_png_image_get_data(self->image);

  // Total data adds a one byte filter type to the start of each row.
  size_t total_data_length = image_height * (1 + row_stride);

  // Pick the smallest window size that can fit all the data.
  size_t window_size = 32768;
  while (window_size > 256 && total_data_length < window_size / 2) {
    window_size /= 2;
  }

  UtObjectRef zlib_input_stream = ut_buffered_input_stream_new();
  UtObjectRef zlib_encoder = ut_zlib_encoder_new_full(
      UT_ZLIB_COMPRESSION_LEVEL_DEFAULT, window_size, zlib_input_stream);
  ut_input_stream_read(zlib_encoder, zlib_data_cb, chunk);

  uint8_t filter_type = 0;
  UtObjectRef filter_data = ut_uint8_array_new_from_data(&filter_type, 1);
  for (size_t row = 0; row < image_height; row++) {
    UtObjectRef row_data =
        ut_list_get_sublist(image_data, row * row_stride, row_stride);
    bool is_last_row = row == image_height - 1;

    ut_buffered_input_stream_write(zlib_input_stream, filter_data, false);
    ut_buffered_input_stream_write(zlib_input_stream, row_data, is_last_row);
  }

  write_chunk(self, chunk);
}

static void write_image_end(UtPngEncoder *self) {
  UtObjectRef chunk = start_chunk(UT_PNG_CHUNK_TYPE_IMAGE_END);
  write_chunk(self, chunk);
}

static void ut_png_encoder_cleanup(UtObject *object) {
  UtPngEncoder *self = (UtPngEncoder *)object;
  ut_object_unref(self->image);
  ut_object_unref(self->output_stream);
}

static UtObjectInterface object_interface = {.type_name = "UtPngEncoder",
                                             .cleanup = ut_png_encoder_cleanup};

UtObject *ut_png_encoder_new(UtObject *image, UtObject *output_stream) {
  UtObject *object = ut_object_new(sizeof(UtPngEncoder), &object_interface);
  UtPngEncoder *self = (UtPngEncoder *)object;
  self->image = ut_object_ref(image);
  self->output_stream = ut_object_ref(output_stream);
  return object;
}

void ut_png_encoder_encode(UtObject *object) {
  assert(ut_object_is_png_encoder(object));
  UtPngEncoder *self = (UtPngEncoder *)object;

  write_signature(self);
  write_image_header(
      self, ut_png_image_get_width(self->image),
      ut_png_image_get_height(self->image),
      ut_png_image_get_bit_depth(self->image),
      encode_color_type(ut_png_image_get_color_type(self->image)),
      COMPRESS_DEFLATE, 0,
      encode_interlace_method(UT_PNG_INTERLACE_METHOD_NONE));
  UtObject *palette = ut_png_image_get_palette(self->image);
  if (palette != NULL) {
    write_palette(self, palette);
  }
  UtObject *background_color = ut_png_image_get_background_color(self->image);
  if (background_color != NULL) {
    write_background(self, background_color);
  }
  write_image_data(self);
  write_image_end(self);
}

bool ut_object_is_png_encoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
