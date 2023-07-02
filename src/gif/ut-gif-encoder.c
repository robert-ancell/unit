#include <assert.h>

#include "ut.h"

// https://www.w3.org/Graphics/GIF/spec-gif89a.txt

typedef struct {
  UtObject object;

  // Dimensions of GIF image.
  uint16_t width;
  uint16_t height;

  // Global color table.
  UtObject *global_color_table;

  // Comments to encode.
  UtObject *comments;

  // Images to encode.
  UtObject *images;

  // Output stream to write to.
  UtObject *output_stream;
} UtGifEncoder;

static size_t get_color_table_depth(UtObject *color_table) {
  size_t color_table_length = ut_list_get_length(color_table) / 3;
  size_t depth = 0;
  while (color_table_length > (1 << depth)) {
    depth++;
  }

  return depth;
}

static void write_header(UtGifEncoder *self) {
  UtObjectRef data =
      ut_uint8_list_new_from_elements(6, 'G', 'I', 'F', '8', '9', 'a');
  ut_output_stream_write(self->output_stream, data);
}

static void write_screen_descriptor(UtGifEncoder *self, uint16_t width,
                                    uint16_t height, uint8_t color_resolution,
                                    uint8_t global_color_table_depth,
                                    bool global_color_table_sorted,
                                    uint8_t background_color_index) {
  assert(color_resolution > 0 && color_resolution <= 8);
  assert(global_color_table_depth <= 8);

  uint8_t flags = 0x00;
  if (global_color_table_depth > 0) {
    flags |= 0x80;
    flags |= global_color_table_depth - 1;
  }
  flags |= (color_resolution - 1) << 4;
  if (global_color_table_sorted) {
    flags |= 0x08;
  }

  UtObjectRef data = ut_uint8_list_new();
  ut_uint8_list_append_uint16_le(data, width);
  ut_uint8_list_append_uint16_le(data, height);
  ut_uint8_list_append(data, flags);
  ut_uint8_list_append(data, background_color_index);
  ut_uint8_list_append(data, 0x00); // pixel aspect ratio
  ut_output_stream_write(self->output_stream, data);
}

static void write_color_table(UtGifEncoder *self, UtObject *color_table,
                              size_t color_table_depth) {
  size_t color_table_length = ut_list_get_length(color_table) / 3;
  size_t n_colors = 1 << color_table_depth;
  assert(color_table_length <= n_colors);
  size_t n_extra_colors = n_colors - color_table_length;

  ut_output_stream_write(self->output_stream, color_table);
  if (n_extra_colors > 0) {
    UtObjectRef data = ut_uint8_list_new();
    ut_list_resize(data, n_extra_colors * 3);
    ut_output_stream_write(self->output_stream, data);
  }
}

static void write_image_descriptor(UtGifEncoder *self, uint16_t left,
                                   uint16_t top, uint16_t width,
                                   uint16_t height, bool interlaced,
                                   uint8_t color_table_depth,
                                   bool color_table_sorted) {
  assert(color_table_depth <= 8);

  uint8_t flags = 0x00;
  if (color_table_depth > 0) {
    flags |= 0x80;
    flags |= color_table_depth - 1;
  }
  if (interlaced) {
    flags |= 0x40;
  }
  if (color_table_sorted) {
    flags |= 0x20;
  }

  UtObjectRef data = ut_uint8_list_new();
  ut_uint8_list_append(data, 0x2c);
  ut_uint8_list_append_uint16_le(data, left);
  ut_uint8_list_append_uint16_le(data, top);
  ut_uint8_list_append_uint16_le(data, width);
  ut_uint8_list_append_uint16_le(data, height);
  ut_uint8_list_append(data, flags);
  ut_output_stream_write(self->output_stream, data);
}

static void write_image_data(UtGifEncoder *self, UtObject *image_data) {
  // Calculate number of color index symbols.
  uint8_t lzw_min_code_size = 0;
  size_t n_symbols = 1 << lzw_min_code_size;
  size_t image_data_length = ut_list_get_length(image_data);
  const uint8_t *image_data_buffer = ut_uint8_list_get_data(image_data);
  for (size_t i = 0; i < image_data_length; i++) {
    uint8_t index = image_data_buffer[i];
    while (index >= n_symbols) {
      lzw_min_code_size++;
      n_symbols = 1 << lzw_min_code_size;
    }
  }

  UtObjectRef data = ut_uint8_list_new();
  ut_uint8_list_append(data, lzw_min_code_size);

  // LZW encode image data.
  UtObjectRef lzw_input_stream = ut_list_input_stream_new(image_data);
  UtObjectRef lzw_encoder =
      ut_lzw_encoder_new_lsb(n_symbols, 4096, lzw_input_stream);
  UtObjectRef lzw_data = ut_input_stream_read_sync(lzw_encoder);

  // Write encoded data in blocks of up to 255 bytes.
  size_t lzw_data_length = ut_list_get_length(lzw_data);
  size_t offset = 0;
  while (offset < lzw_data_length) {
    size_t sub_block_length = lzw_data_length - offset;
    if (sub_block_length > 255) {
      sub_block_length = 255;
    }

    UtObjectRef sub_block =
        ut_list_get_sublist(lzw_data, offset, sub_block_length);
    ut_uint8_list_append(data, sub_block_length);
    ut_list_append_list(data, sub_block);

    offset += sub_block_length;
  }
  // Trailing sub-block of zero length;
  ut_uint8_list_append(data, 0);

  ut_output_stream_write(self->output_stream, data);
}

static void write_extension_header(UtObject *data, uint8_t label) {
  ut_uint8_list_append(data, 0x21);
  ut_uint8_list_append(data, label);
}

static void write_graphic_control_extension(UtGifEncoder *self,
                                            uint8_t disposal_method,
                                            bool user_input,
                                            uint16_t delay_time,
                                            int transparent_color_index) {
  assert(disposal_method < 8);
  assert(transparent_color_index < 256);

  UtObjectRef data = ut_uint8_list_new();

  uint8_t flags = 0x00;
  flags |= disposal_method << 2;
  if (user_input) {
    flags |= 0x02;
  }
  if (transparent_color_index >= 0) {
    flags |= 0x01;
  }

  write_extension_header(data, 0xf9);

  // Block 0
  ut_uint8_list_append(data, 4);
  ut_uint8_list_append(data, flags);
  ut_uint8_list_append_uint16_le(data, delay_time);
  ut_uint8_list_append(
      data, transparent_color_index >= 0 ? transparent_color_index : 0);

  // Terminator block
  ut_uint8_list_append(data, 0);

  ut_output_stream_write(self->output_stream, data);
}

static void write_comment_extension(UtGifEncoder *self, const char *comment) {
  UtObjectRef data = ut_uint8_list_new();
  write_extension_header(data, 0xfe);

  size_t comment_length = ut_cstring_get_length(comment);
  size_t offset = 0;
  while (offset < comment_length) {
    size_t block_length = comment_length - offset;
    if (block_length > 255) {
      block_length = 255;
    }
    ut_uint8_list_append(data, block_length);
    for (size_t i = 0; i < block_length; i++) {
      ut_uint8_list_append(data, comment[offset + i]);
    }

    offset += block_length;
  }

  // Terminator block
  ut_uint8_list_append(data, 0);

  ut_output_stream_write(self->output_stream, data);
}

static void write_trailer(UtGifEncoder *self) {
  UtObjectRef data = ut_uint8_list_new_from_elements(1, 0x3b);
  ut_output_stream_write(self->output_stream, data);
}

static void ut_gif_encoder_init(UtObject *object) {
  UtGifEncoder *self = (UtGifEncoder *)object;
  self->comments = ut_string_list_new();
}

static void ut_gif_encoder_cleanup(UtObject *object) {
  UtGifEncoder *self = (UtGifEncoder *)object;
  ut_object_unref(self->global_color_table);
  ut_object_unref(self->comments);
  ut_object_unref(self->images);
  ut_object_unref(self->output_stream);
}

static UtObjectInterface object_interface = {.type_name = "UtGifEncoder",
                                             .init = ut_gif_encoder_init,
                                             .cleanup = ut_gif_encoder_cleanup};

UtObject *ut_gif_encoder_new(uint16_t width, uint16_t height,
                             UtObject *global_color_table, UtObject *images,
                             UtObject *output_stream) {
  UtObject *object = ut_object_new(sizeof(UtGifEncoder), &object_interface);
  UtGifEncoder *self = (UtGifEncoder *)object;

  assert(global_color_table == NULL ||
         ut_list_get_length(global_color_table) % 3 == 0);

  self->width = width;
  self->height = height;
  self->global_color_table = ut_object_ref(global_color_table);
  self->images = ut_object_ref(images);
  self->output_stream = ut_object_ref(output_stream);
  return object;
}

UtObject *ut_gif_encoder_new_single(UtObject *image, UtObject *output_stream) {
  UtObjectRef images = ut_list_new();
  ut_list_append(images, image);
  return ut_gif_encoder_new(
      ut_gif_image_get_left(image) + ut_gif_image_get_width(image),
      ut_gif_image_get_top(image) + ut_gif_image_get_height(image),
      ut_gif_image_get_color_table(image), images, output_stream);
}

void ut_gif_encoder_add_comment(UtObject *object, const char *comment) {
  assert(ut_object_is_gif_encoder(object));
  UtGifEncoder *self = (UtGifEncoder *)object;

  ut_string_list_append(self->comments, comment);
}

void ut_gif_encoder_encode(UtObject *object) {
  assert(ut_object_is_gif_encoder(object));
  UtGifEncoder *self = (UtGifEncoder *)object;

  size_t global_color_table_depth = 0;
  if (self->global_color_table != NULL) {
    global_color_table_depth = get_color_table_depth(self->global_color_table);
  }

  write_header(self);
  write_screen_descriptor(self, self->width, self->height, 8,
                          global_color_table_depth, false, 0);
  if (self->global_color_table != NULL) {
    write_color_table(self, self->global_color_table, global_color_table_depth);
  }

  size_t comments_length = ut_list_get_length(self->comments);
  for (size_t i = 0; i < comments_length; i++) {
    write_comment_extension(self,
                            ut_string_list_get_element(self->comments, i));
  }

  size_t images_length = ut_list_get_length(self->images);
  for (size_t i = 0; i < images_length; i++) {
    UtObjectRef image = ut_list_get_element(self->images, i);

    UtGifDisposalMethod disposal_method =
        ut_gif_image_get_disposal_method(image);
    uint16_t delay_time = ut_gif_image_get_delay_time(image);
    if (disposal_method != UT_GIF_DISPOSAL_METHOD_NONE || delay_time != 0) {
      write_graphic_control_extension(self, disposal_method, false, delay_time,
                                      -1);
    }

    write_image_descriptor(self, ut_gif_image_get_left(image),
                           ut_gif_image_get_top(image),
                           ut_gif_image_get_width(image),
                           ut_gif_image_get_height(image), false, 0, false);
    write_image_data(self, ut_gif_image_get_data(image));
  }
  write_trailer(self);
}

bool ut_object_is_gif_encoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
