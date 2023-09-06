#include <assert.h>

#include "ut.h"

// https://www.w3.org/Graphics/GIF/spec-gif89a.txt

typedef enum {
  DECODER_STATE_HEADER,
  DECODER_STATE_SCREEN_DESCRIPTOR,
  DECODER_STATE_COLOR_TABLE,
  DECODER_STATE_BLOCK,
  DECODER_STATE_EXTENSION,
  DECODER_STATE_IMAGE,
  DECODER_STATE_IMAGE_COLOR_TABLE,
  DECODER_STATE_IMAGE_DATA,
  DECODER_STATE_IMAGE_DATA_BLOCK,
  DECODER_STATE_ERROR,
  DECODER_STATE_END
} DecoderState;

typedef struct {
  UtObject object;

  // Input stream being read.
  UtObject *input_stream;

  // Callback to notify when complete.
  UtObject *callback_object;
  UtGifDecodeCallback callback;

  // Current state of the decoder.
  DecoderState state;

  // Dimensions of image.
  uint16_t width;
  uint16_t height;

  // Number of times to loop animation.
  size_t loop_count;

  // True if the global color table is sorted in decreasing importance;
  bool global_color_table_sorted;

  // Original color depth of image.
  uint8_t color_resolution;

  // Image background color.
  uint8_t background_color_index;

  // Size of the color table to be decoded.
  size_t color_table_length;

  // Color table used for all images.
  UtObject *global_color_table;

  // Color table used in current image.
  UtObject *image_color_table;

  // Data in current image.
  UtObject *image_data;
  size_t image_read_count;

  // Dimensions of current image.
  uint16_t image_left;
  uint16_t image_top;
  uint16_t image_width;
  uint16_t image_height;

  // Row order if interlaced.
  UtObject *interlace_order;

  // Disposal method to render next image.
  UtGifDisposalMethod disposal_method;

  // Delay time before showing the next image.
  uint16_t delay_time;

  // LZW decoder for image data.
  UtObject *lzw_input_stream;
  UtObject *lzw_decoder;

  // Comments in the file.
  UtObject *comments;

  // Images in the file.
  UtObject *images;

  // Error that occurred during decoding.
  UtObject *error;
} UtGifDecoder;

static void notify_complete(UtGifDecoder *self) {
  ut_input_stream_close(self->input_stream);
  if (self->callback_object != NULL) {
    self->callback(self->callback_object);
  }
}

static void set_error(UtGifDecoder *self, const char *description) {
  if (self->state == DECODER_STATE_ERROR) {
    return;
  }

  self->error = ut_gif_error_new(description);
  self->state = DECODER_STATE_ERROR;
  notify_complete(self);
}

static void set_error_take(UtGifDecoder *self, char *description) {
  set_error(self, description);
  free(description);
}

static size_t decode_header(UtGifDecoder *self, UtObject *data) {
  if (ut_list_get_length(data) < 6) {
    return 0;
  }

  if (ut_uint8_list_get_element(data, 0) != 'G' ||
      ut_uint8_list_get_element(data, 1) != 'I' ||
      ut_uint8_list_get_element(data, 2) != 'F') {
    set_error(self, "Invalid GIF signature");
    return 0;
  }

  uint8_t v0 = ut_uint8_list_get_element(data, 3);
  uint8_t v1 = ut_uint8_list_get_element(data, 4);
  uint8_t v2 = ut_uint8_list_get_element(data, 5);
  if (v0 == '8' && v1 == '9' && v2 == 'a') {
  } else if (v0 == '8' && v1 == '7' && v2 == 'a') {
  } else {
    set_error(self, "Invalid GIF version");
    return 0;
  }

  self->state = DECODER_STATE_SCREEN_DESCRIPTOR;
  return 6;
}

static size_t decode_screen_descriptor(UtGifDecoder *self, UtObject *data) {
  if (ut_list_get_length(data) < 7) {
    return 0;
  }

  self->width = ut_uint8_list_get_uint16_le(data, 0);
  self->height = ut_uint8_list_get_uint16_le(data, 2);
  uint8_t flags = ut_uint8_list_get_element(data, 4);
  self->background_color_index = ut_uint8_list_get_element(data, 5);
  // uint8_t pixel_aspect_ratio = ut_uint8_list_get_element(data, 6);

  if (self->width == 0 || self->height == 0) {
    set_error(self, "Invalid GIF image dimensions");
    return 0;
  }

  bool has_global_color_table = (flags & 0x80) != 0;
  self->color_resolution = ((flags >> 4) & 0x7) + 1;
  self->global_color_table_sorted = (flags & 0x08) != 0;
  uint8_t global_color_table_depth = (flags & 0x7) + 1;

  if (has_global_color_table) {
    self->color_table_length = 1 << global_color_table_depth;
    self->state = DECODER_STATE_COLOR_TABLE;
  } else {
    self->state = DECODER_STATE_BLOCK;
  }

  return 7;
}

static size_t decode_color_table(UtGifDecoder *self, UtObject *data) {
  if (ut_list_get_length(data) < self->color_table_length * 3) {
    return 0;
  }

  UtObjectRef color_table =
      ut_list_get_sublist(data, 0, self->color_table_length * 3);
  self->global_color_table = ut_list_copy(color_table);

  self->state = DECODER_STATE_BLOCK;

  return self->color_table_length * 3;
}

static size_t decode_block(UtGifDecoder *self, UtObject *data) {
  if (ut_list_get_length(data) < 1) {
    return 0;
  }

  uint8_t block_type = ut_uint8_list_get_element(data, 0);
  switch (block_type) {
  case 0x21:
    self->state = DECODER_STATE_EXTENSION;
    break;
  case 0x2c:
    self->state = DECODER_STATE_IMAGE;
    break;
  case 0x3b:
    self->state = DECODER_STATE_END;
    notify_complete(self);
    break;
  default:
    set_error(self, "Invalid GIF block type");
    return 0;
  }

  return 1;
}

static void decode_comment(UtGifDecoder *self, UtObject *sub_blocks) {
  // Merge sub-blocks into one.
  size_t sub_blocks_length = ut_list_get_length(sub_blocks);
  UtObjectRef comment_data = NULL;
  if (sub_blocks_length == 1) {
    comment_data = ut_list_get_element(sub_blocks, 0);
  } else {
    comment_data = ut_uint8_list_new();
    for (size_t i = 0; i < sub_blocks_length; i++) {
      ut_list_append_list(comment_data,
                          ut_object_list_get_element(sub_blocks, i));
    }
  }

  UtObjectRef comment = ut_string_new_from_ascii(comment_data);
  if (ut_object_implements_error(comment)) {
    set_error_take(self,
                   ut_cstring_new_printf("Invalid GIF comment: %s",
                                         ut_error_get_description(comment)));
    return;
  }

  ut_string_list_append(self->comments, ut_string_get_text(comment));
}

static void decode_graphic_control(UtGifDecoder *self, UtObject *sub_blocks) {
  if (ut_list_get_length(sub_blocks) != 1) {
    set_error(self, "Invalid graphical control extension sub block count");
    return;
  }
  UtObjectRef data = ut_list_get_element(sub_blocks, 0);
  if (ut_list_get_length(data) != 4) {
    set_error(self, "Invalid graphical control extension length");
    return;
  }

  uint8_t flags = ut_uint8_list_get_element(data, 0);
  self->delay_time = ut_uint8_list_get_uint16_le(data, 1);
  // uint8_t transparent_color_index = ut_uint8_list_get_element(data, 3);

  self->disposal_method = (flags >> 2) & 0x7;
  // uint8_t user_input = (flags & 0x2) != 0;
  // uint8_t has_transparent_color = (flags & 0x1) != 0;
}

static void decode_netscape_extension(UtGifDecoder *self,
                                      UtObject *authentication_code,
                                      UtObject *sub_blocks) {
  if (ut_uint8_list_get_element(authentication_code, 0) != '2' ||
      ut_uint8_list_get_element(authentication_code, 1) != '.' ||
      ut_uint8_list_get_element(authentication_code, 2) != '0') {
    set_error(self, "Invalid GIF NETSCAPE extension authentication code");
    return;
  }

  size_t sub_blocks_length = ut_list_get_length(sub_blocks);
  for (size_t i = 0; i < sub_blocks_length; i++) {
    UtObject *b = ut_object_list_get_element(sub_blocks, i);
    uint8_t id = ut_uint8_list_get_element(b, 0);
    if (id == 1) {
      if (ut_list_get_length(b) != 3) {
        set_error(self, "Invalid loop count in GIF NETSCAPE extension");
        return;
      }
      self->loop_count = ut_uint8_list_get_uint16_le(b, 1);
    }
  }
}

static void decode_application_extension(UtGifDecoder *self,
                                         UtObject *sub_blocks) {
  size_t sub_blocks_length = ut_list_get_length(sub_blocks);
  if (sub_blocks_length < 1) {
    set_error(self,
              "Missing application identifier in GIF application extension");
    return;
  }

  UtObject *identifier_block = ut_object_list_get_element(sub_blocks, 0);
  if (ut_list_get_length(identifier_block) != 11) {
    set_error(
        self,
        "Invalid length application identifier in GIF application extension");
    return;
  }

  UtObjectRef identifier_data = ut_list_get_sublist(identifier_block, 0, 8);
  UtObjectRef authentication_code = ut_list_get_sublist(identifier_block, 8, 3);
  UtObjectRef identifier = ut_string_new_from_ascii(identifier_data);
  if (ut_object_implements_error(identifier)) {
    ut_cstring_ref description = ut_cstring_new_printf(
        "Invalid GIF comment: %s", ut_error_get_description(identifier));
    set_error(self,
              "Invalid application identifier in GIF application extension");
    return;
  }

  const char *identifier_text = ut_string_get_text(identifier);
  UtObjectRef application_sub_blocks =
      ut_list_get_sublist(sub_blocks, 1, sub_blocks_length - 1);
  if (ut_cstring_equal(identifier_text, "NETSCAPE")) {
    decode_netscape_extension(self, authentication_code,
                              application_sub_blocks);
  }
}

static size_t decode_extension(UtGifDecoder *self, UtObject *data) {
  size_t data_length = ut_list_get_length(data);
  if (data_length < 2) {
    return 0;
  }

  uint8_t label = ut_uint8_list_get_element(data, 0);
  size_t offset = 1;
  UtObjectRef sub_blocks = ut_object_list_new();
  while (offset < data_length) {
    uint8_t sub_block_length = ut_uint8_list_get_element(data, offset);
    size_t sub_block_start = offset + 1;
    offset += 1 + sub_block_length;
    if (sub_block_length == 0) {
      self->state = DECODER_STATE_BLOCK;
      break;
    }

    UtObjectRef sub_block =
        ut_list_get_sublist(data, sub_block_start, sub_block_length);
    ut_list_append(sub_blocks, sub_block);
  }

  switch (label) {
  case 0xfe:
    decode_comment(self, sub_blocks);
    break;
  case 0xf9:
    decode_graphic_control(self, sub_blocks);
    break;
  case 0xff:
    decode_application_extension(self, sub_blocks);
    break;
  }

  return offset;
}

static size_t decode_image(UtGifDecoder *self, UtObject *data) {
  if (ut_list_get_length(data) < 9) {
    return 0;
  }

  self->image_left = ut_uint8_list_get_uint16_le(data, 0);
  self->image_top = ut_uint8_list_get_uint16_le(data, 2);
  self->image_width = ut_uint8_list_get_uint16_le(data, 4);
  self->image_height = ut_uint8_list_get_uint16_le(data, 6);
  uint8_t flags = ut_uint8_list_get_element(data, 8);

  bool has_color_table = (flags & 0x80) != 0;
  bool is_interlaced = (flags & 0x40) != 0;
  uint8_t color_table_depth = (flags & 0x7) + 1;

  if (is_interlaced) {
    self->interlace_order = ut_uint16_list_new();
    for (size_t row = 0; row < self->image_height; row += 8) {
      ut_uint16_list_append(self->interlace_order, row);
    }
    for (size_t row = 4; row < self->image_height; row += 8) {
      ut_uint16_list_append(self->interlace_order, row);
    }
    for (size_t row = 2; row < self->image_height; row += 4) {
      ut_uint16_list_append(self->interlace_order, row);
    }
    for (size_t row = 1; row < self->image_height; row += 2) {
      ut_uint16_list_append(self->interlace_order, row);
    }
  } else {
    ut_object_unref(self->interlace_order);
    self->interlace_order = NULL;
  }

  if (has_color_table) {
    self->color_table_length = 1 << color_table_depth;
    self->state = DECODER_STATE_IMAGE_COLOR_TABLE;
  } else {
    if (self->global_color_table == NULL) {
      set_error(self, "Missing color table");
      return 0;
    }
    ut_object_clear(&self->image_color_table);
    self->image_color_table = ut_object_ref(self->global_color_table);
    self->state = DECODER_STATE_IMAGE_DATA;
  }

  return 9;
}

static size_t decode_image_color_table(UtGifDecoder *self, UtObject *data) {
  if (ut_list_get_length(data) < self->color_table_length * 3) {
    return 0;
  }

  ut_object_clear(&self->image_color_table);
  UtObjectRef color_table =
      ut_list_get_sublist(data, 0, self->color_table_length * 3);
  self->image_color_table = ut_list_copy(color_table);

  self->state = DECODER_STATE_IMAGE_DATA;

  return self->color_table_length * 3;
}

static size_t lzw_read_cb(UtObject *object, UtObject *data, bool complete) {
  UtGifDecoder *self = (UtGifDecoder *)object;

  if (ut_object_implements_error(data)) {
    set_error_take(self, ut_cstring_new_printf("Image data LZW error: %s",
                                               ut_error_get_description(data)));
    return 0;
  }

  uint8_t *image_data = ut_uint8_list_get_writable_data(self->image_data);
  size_t image_data_length = ut_list_get_length(self->image_data);

  size_t color_table_length = ut_list_get_length(self->image_color_table);

  size_t data_length = ut_list_get_length(data);
  for (size_t i = 0; i < data_length; i++) {
    if (self->image_read_count >= image_data_length) {
      set_error(self, "Excess pixels in GIF image");
      return 0;
    }
    uint8_t color_index = ut_uint8_list_get_element(data, i);
    if (color_index >= color_table_length) {
      set_error(self, "Invalid color index in GIF image");
      return 0;
    }

    if (self->interlace_order != NULL) {
      size_t x = self->image_read_count % self->image_width;
      size_t y = self->image_read_count / self->image_width;
      y = ut_uint16_list_get_element(self->interlace_order, y);
      image_data[(y * self->image_width) + x] = color_index;
    } else {
      image_data[self->image_read_count] = color_index;
    }

    self->image_read_count++;
  }

  if (complete && self->image_read_count != image_data_length) {
    set_error(self, "Insufficient pixels in GIF image");
    return 0;
  }

  return ut_list_get_length(data);
}

static size_t decode_image_data(UtGifDecoder *self, UtObject *data) {
  if (ut_list_get_length(data) < 1) {
    return 0;
  }

  ut_object_unref(self->image_data);
  self->image_data = ut_uint8_list_new();
  ut_list_resize(self->image_data, self->image_width * self->image_height);
  self->image_read_count = 0;

  UtObjectRef image = ut_gif_image_new(
      self->image_left, self->image_top, self->image_width, self->image_height,
      self->image_color_table, self->image_data);
  ut_gif_image_set_disposal_method(image, self->disposal_method);
  ut_gif_image_set_delay_time(image, self->delay_time);
  ut_list_append(self->images, image);

  uint8_t lzw_min_code_size = ut_uint8_list_get_element(data, 0);
  ut_object_unref(self->lzw_input_stream);
  ut_object_unref(self->lzw_decoder);
  self->lzw_input_stream = ut_writable_input_stream_new();
  self->lzw_decoder = ut_lzw_decoder_new_lsb(1 << lzw_min_code_size, 4096,
                                             self->lzw_input_stream);
  ut_input_stream_read(self->lzw_decoder, (UtObject *)self, lzw_read_cb);

  self->state = DECODER_STATE_IMAGE_DATA_BLOCK;

  return 1;
}

static size_t decode_image_data_block(UtGifDecoder *self, UtObject *data) {
  if (ut_list_get_length(data) < 1) {
    return 0;
  }

  uint8_t sub_block_length = ut_uint8_list_get_element(data, 0);
  if (ut_list_get_length(data) < 1 + sub_block_length) {
    return 0;
  }

  UtObjectRef sub_block = ut_list_get_sublist(data, 1, sub_block_length);
  ut_writable_input_stream_write(self->lzw_input_stream, sub_block,
                                 sub_block_length == 0);

  if (sub_block_length == 0) {
    self->disposal_method = 0;
    self->delay_time = 0;

    self->state = DECODER_STATE_BLOCK;
  }

  return 1 + sub_block_length;
}

static size_t read_cb(UtObject *object, UtObject *data, bool complete) {
  UtGifDecoder *self = (UtGifDecoder *)object;

  if (ut_object_implements_error(data)) {
    set_error_take(self, ut_cstring_new_printf("Failed to read GIF data: %s",
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
    case DECODER_STATE_HEADER:
      n_used = decode_header(self, d);
      break;
    case DECODER_STATE_SCREEN_DESCRIPTOR:
      n_used = decode_screen_descriptor(self, d);
      break;
    case DECODER_STATE_COLOR_TABLE:
      n_used = decode_color_table(self, d);
      break;
    case DECODER_STATE_BLOCK:
      n_used = decode_block(self, d);
      break;
    case DECODER_STATE_EXTENSION:
      n_used = decode_extension(self, d);
      break;
    case DECODER_STATE_IMAGE:
      n_used = decode_image(self, d);
      break;
    case DECODER_STATE_IMAGE_COLOR_TABLE:
      n_used = decode_image_color_table(self, d);
      break;
    case DECODER_STATE_IMAGE_DATA:
      n_used = decode_image_data(self, d);
      break;
    case DECODER_STATE_IMAGE_DATA_BLOCK:
      n_used = decode_image_data_block(self, d);
      break;
    case DECODER_STATE_ERROR:
    case DECODER_STATE_END:
      return offset;
    default:
      assert(false);
    }

    if (self->state == old_state && n_used == 0) {
      if (complete && self->state != DECODER_STATE_END) {
        set_error(self, "Incomplete GIF");
      }
      return offset;
    }

    offset += n_used;
  }
}

static void done_cb(UtObject *object) {}

static void ut_gif_decoder_init(UtObject *object) {
  UtGifDecoder *self = (UtGifDecoder *)object;
  self->state = DECODER_STATE_HEADER;
  self->loop_count = 1;
  self->comments = ut_string_list_new();
  self->images = ut_object_list_new();
}

static void ut_gif_decoder_cleanup(UtObject *object) {
  UtGifDecoder *self = (UtGifDecoder *)object;

  ut_input_stream_close(self->input_stream);

  ut_object_unref(self->input_stream);
  ut_object_weak_unref(&self->callback_object);
  ut_object_unref(self->global_color_table);
  ut_object_unref(self->image_color_table);
  ut_object_unref(self->image_data);
  ut_object_unref(self->interlace_order);
  ut_object_unref(self->lzw_input_stream);
  ut_object_unref(self->lzw_decoder);
  ut_object_unref(self->comments);
  ut_object_unref(self->images);
  ut_object_unref(self->error);
}

static UtObjectInterface object_interface = {.type_name = "UtGifDecoder",
                                             .init = ut_gif_decoder_init,
                                             .cleanup = ut_gif_decoder_cleanup};

UtObject *ut_gif_decoder_new(UtObject *input_stream) {
  UtObject *object = ut_object_new(sizeof(UtGifDecoder), &object_interface);
  UtGifDecoder *self = (UtGifDecoder *)object;
  self->input_stream = ut_object_ref(input_stream);
  return object;
}

void ut_gif_decoder_decode(UtObject *object, UtObject *callback_object,
                           UtGifDecodeCallback callback) {
  assert(ut_object_is_gif_decoder(object));
  UtGifDecoder *self = (UtGifDecoder *)object;

  assert(self->callback == NULL);
  assert(callback != NULL);

  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callback = callback;

  ut_input_stream_read(self->input_stream, object, read_cb);
}

UtObject *ut_gif_decoder_decode_sync(UtObject *object) {
  assert(ut_object_is_gif_decoder(object));
  UtGifDecoder *self = (UtGifDecoder *)object;

  UtObjectRef dummy_object = ut_null_new();
  ut_gif_decoder_decode(object, dummy_object, done_cb);
  if (self->error != NULL) {
    return ut_object_ref(self->error);
  }

  return ut_object_ref(self->images);
}

UtObject *ut_gif_decoder_get_error(UtObject *object) {
  assert(ut_object_is_gif_decoder(object));
  UtGifDecoder *self = (UtGifDecoder *)object;
  return self->error;
}

uint16_t ut_gif_decoder_get_width(UtObject *object) {
  assert(ut_object_is_gif_decoder(object));
  UtGifDecoder *self = (UtGifDecoder *)object;
  return self->width;
}

uint16_t ut_gif_decoder_get_height(UtObject *object) {
  assert(ut_object_is_gif_decoder(object));
  UtGifDecoder *self = (UtGifDecoder *)object;
  return self->height;
}

UtObject *ut_gif_decoder_get_comments(UtObject *object) {
  assert(ut_object_is_gif_decoder(object));
  UtGifDecoder *self = (UtGifDecoder *)object;
  return self->comments;
}

UtObject *ut_gif_decoder_get_images(UtObject *object) {
  assert(ut_object_is_gif_decoder(object));
  UtGifDecoder *self = (UtGifDecoder *)object;
  return self->images;
}

size_t ut_gif_decoder_get_loop_count(UtObject *object) {
  assert(ut_object_is_gif_decoder(object));
  UtGifDecoder *self = (UtGifDecoder *)object;
  return self->loop_count;
}

bool ut_object_is_gif_decoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
