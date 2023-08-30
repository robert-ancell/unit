#include <assert.h>

#include "ut.h"

// Code used for blocks that use static Huffman codes.
#define BLOCK_STATIC_HUFFMAN 1

// Symbol used for end of stream.
#define END_OF_STREAM 256

typedef struct {
  UtObject object;
  UtObject *input_stream;
  UtObject *callback_object;
  UtInputStreamCallback callback;

  size_t window_size;

  // Huffman encoder for literal/length codes.
  UtObject *literal_length_huffman_encoder;

  // Huffman necoder for distance codes.
  UtObject *distance_huffman_encoder;

  // Dictionary of sent data.
  UtObject *dictionary;

  // Encoded data buffer.
  bool written_header;
  UtObject *buffer;
  size_t partial_byte;
  size_t partial_byte_length;
} UtDeflateEncoder;

// Write remaining partial byte to the buffer.
static void end_bits(UtDeflateEncoder *self) {
  if (self->partial_byte_length != 0) {
    ut_uint8_list_append(self->buffer, self->partial_byte);
    self->partial_byte = 0x00;
    self->partial_byte_length = 0;
  }
}

// Append a bit to the buffer.
static void write_bit(UtDeflateEncoder *self, uint8_t value) {
  self->partial_byte |= value << self->partial_byte_length;
  self->partial_byte_length++;
  if (self->partial_byte_length == 8) {
    ut_uint8_list_append(self->buffer, self->partial_byte);
    self->partial_byte = 0x00;
    self->partial_byte_length = 0;
  }
}

// Write an integer to the buffer.
static void write_int(UtDeflateEncoder *self, uint16_t value,
                      size_t value_length) {
  for (size_t i = 0; i < value_length; i++) {
    write_bit(self, value & 0x1);
    value >>= 1;
  }
}

// Write a deflate block header.
static void write_block_header(UtDeflateEncoder *self, bool is_last_block,
                               uint8_t block_type) {
  write_bit(self, is_last_block ? 1 : 0);
  write_int(self, block_type, 2);
}

// Write a Huffman encoded symbol.
static void write_symbol(UtDeflateEncoder *self, UtObject *huffman_encoder,
                         uint16_t symbol) {
  uint16_t code;
  size_t code_width;
  ut_huffman_encoder_get_code(huffman_encoder, symbol, &code, &code_width);
  for (size_t i = 0; i < code_width; i++) {
    write_bit(self, (code >> (code_width - i - 1)) & 0x1);
  }
}

// Find a match in the dictionary.
static void find_match(UtDeflateEncoder *self, size_t data_length,
                       size_t *best_distance, size_t *best_length) {
  const uint8_t *dictionary = ut_uint8_list_get_data(self->dictionary);
  size_t dictionary_length = ut_list_get_length(self->dictionary);
  const uint8_t *data = dictionary + dictionary_length - data_length;

  size_t max_distance = dictionary_length - data_length;
  if (max_distance >= self->window_size) {
    max_distance = self->window_size;
  }

  size_t best_distance_ = 0;
  size_t best_length_ = 0;
  for (size_t distance = 1; distance <= max_distance; distance++) {
    const uint8_t *match = data - distance;

    // Can't match more than the data we have.
    size_t max_length = data_length;
    if (max_length > 285) {
      max_length = 285;
    }

    size_t length;
    for (length = 0; length < max_length; length++) {
      if (match[length] != data[length]) {
        break;
      }
    }

    if (length > best_length_) {
      best_distance_ = distance;
      best_length_ = length;
    }
  }

  *best_distance = best_distance_;
  *best_length = best_length_;
}

static void write_literal(UtDeflateEncoder *self, uint8_t value) {
  write_symbol(self, self->literal_length_huffman_encoder, value);
}

// Write a repeat length.
static void write_length(UtDeflateEncoder *self, size_t length) {
  if (length <= 2) {
    assert(false);
  } else if (length <= 10) {
    write_symbol(self, self->literal_length_huffman_encoder, 257 + length - 3);
  } else if (length <= 18) {
    write_symbol(self, self->literal_length_huffman_encoder,
                 265 + ((length - 11) >> 1));
    write_int(self, (length - 11) % 2, 1);
  } else if (length <= 34) {
    write_symbol(self, self->literal_length_huffman_encoder,
                 269 + ((length - 19) >> 2));
    write_int(self, (length - 19) % 4, 2);
  } else if (length <= 66) {
    write_symbol(self, self->literal_length_huffman_encoder,
                 273 + ((length - 35) >> 3));
    write_int(self, (length - 35) % 8, 3);
  } else if (length <= 130) {
    write_symbol(self, self->literal_length_huffman_encoder,
                 277 + ((length - 67) >> 4));
    write_int(self, (length - 67) % 16, 4);
  } else if (length <= 257) {
    write_symbol(self, self->literal_length_huffman_encoder,
                 281 + ((length - 131) >> 5));
    write_int(self, (length - 131) % 32, 5);
  } else if (length == 258) {
    write_symbol(self, self->literal_length_huffman_encoder, 285);
  } else {
    assert(false);
  }
}

// Write a repeat distance.
static void write_distance(UtDeflateEncoder *self, size_t distance) {
  if (distance < 1) {
    assert(false);
  } else if (distance <= 4) {
    write_symbol(self, self->distance_huffman_encoder, distance - 1);
  } else if (distance <= 8) {
    write_symbol(self, self->distance_huffman_encoder,
                 4 + ((distance - 5) >> 1));
    write_int(self, (distance - 5) % 2, 1);
  } else if (distance <= 16) {
    write_symbol(self, self->distance_huffman_encoder,
                 6 + ((distance - 9) >> 2));
    write_int(self, (distance - 9) % 4, 2);
  } else if (distance <= 32) {
    write_symbol(self, self->distance_huffman_encoder,
                 8 + ((distance - 17) >> 3));
    write_int(self, (distance - 17) % 8, 3);
  } else if (distance <= 64) {
    write_symbol(self, self->distance_huffman_encoder,
                 10 + ((distance - 33) >> 4));
    write_int(self, (distance - 33) % 16, 4);
  } else if (distance <= 128) {
    write_symbol(self, self->distance_huffman_encoder,
                 12 + ((distance - 65) >> 5));
    write_int(self, (distance - 65) % 32, 5);
  } else if (distance <= 256) {
    write_symbol(self, self->distance_huffman_encoder,
                 14 + ((distance - 129) >> 6));
    write_int(self, (distance - 129) % 64, 6);
  } else if (distance <= 512) {
    write_symbol(self, self->distance_huffman_encoder,
                 16 + ((distance - 257) >> 7));
    write_int(self, (distance - 257) % 128, 7);
  } else if (distance <= 1024) {
    write_symbol(self, self->distance_huffman_encoder,
                 18 + ((distance - 513) >> 8));
    write_int(self, (distance - 513) % 256, 8);
  } else if (distance <= 2048) {
    write_symbol(self, self->distance_huffman_encoder,
                 20 + ((distance - 1025) >> 9));
    write_int(self, (distance - 1025) % 512, 9);
  } else if (distance <= 4096) {
    write_symbol(self, self->distance_huffman_encoder,
                 22 + ((distance - 2049) >> 10));
    write_int(self, (distance - 2049) % 1024, 10);
  } else if (distance <= 8192) {
    write_symbol(self, self->distance_huffman_encoder,
                 24 + ((distance - 4097) >> 11));
    write_int(self, (distance - 4097) % 2048, 11);
  } else if (distance <= 16384) {
    write_symbol(self, self->distance_huffman_encoder,
                 26 + ((distance - 8193) >> 12));
    write_int(self, (distance - 8193) % 4096, 12);
  } else if (distance <= 32768) {
    write_symbol(self, self->distance_huffman_encoder,
                 28 + ((distance - 16385) >> 13));
    write_int(self, (distance - 16385) % 8192, 13);
  } else {
    assert(false);
  }
}

static size_t read_cb(UtObject *object, UtObject *data, bool complete) {
  UtDeflateEncoder *self = (UtDeflateEncoder *)object;

  if (!self->written_header) {
    write_block_header(self, true, BLOCK_STATIC_HUFFMAN);
    self->written_header = true;
  }

  size_t orig_dictionary_length = ut_list_get_length(self->dictionary);
  ut_list_append_list(self->dictionary, data);
  const uint8_t *dictionary_data = ut_uint8_list_get_data(self->dictionary);
  const uint8_t *new_data = dictionary_data + orig_dictionary_length;

  // TODO: Can trim dictionary to `window_size`

  size_t data_length = ut_list_get_length(data);
  size_t n_used = 0;
  while (n_used < data_length) {
    size_t distance, length;
    find_match(self, data_length - n_used, &distance, &length);

    // If we have a match for all the data, stop here if more data to come - the
    // match might be longer.
    if (!complete && length == n_used) {
      ut_list_remove(self->dictionary, orig_dictionary_length + n_used,
                     data_length - n_used);
      break;
    }

    if (length >= 3) {
      write_length(self, length);
      write_distance(self, distance);
      n_used += length;
    } else {
      write_literal(self, new_data[n_used]);
      n_used++;
    }
  }

  // If complete, use partially filled bytes.
  if (complete) {
    write_symbol(self, self->literal_length_huffman_encoder, END_OF_STREAM);
    end_bits(self);
  }

  if (ut_list_get_length(self->buffer) > 0 || complete) {
    size_t n =
        self->callback_object != NULL
            ? self->callback(self->callback_object, self->buffer, complete)
            : 0;
    ut_list_remove(self->buffer, 0, n);
  }

  return n_used;
}

static void ut_deflate_encoder_init(UtObject *object) {
  UtDeflateEncoder *self = (UtDeflateEncoder *)object;
  self->dictionary = ut_uint8_array_new();
  self->buffer = ut_uint8_array_new();

  UtObjectRef literal_length_code_widths = ut_uint8_list_new();
  for (size_t symbol = 0; symbol <= 287; symbol++) {
    uint8_t code_width;
    if (symbol <= 143) {
      code_width = 8;
    } else if (symbol <= 255) {
      code_width = 9;
    } else if (symbol <= 279) {
      code_width = 7;
    } else {
      code_width = 8;
    }
    ut_uint8_list_append(literal_length_code_widths, code_width);
  }
  self->literal_length_huffman_encoder =
      ut_huffman_encoder_new_canonical(literal_length_code_widths);

  UtObjectRef distance_code_widths = ut_uint8_list_new();
  for (size_t symbol = 0; symbol < 32; symbol++) {
    ut_uint8_list_append(distance_code_widths, 5);
  }
  self->distance_huffman_encoder =
      ut_huffman_encoder_new_canonical(distance_code_widths);
}

static void ut_deflate_encoder_cleanup(UtObject *object) {
  UtDeflateEncoder *self = (UtDeflateEncoder *)object;

  ut_input_stream_close(self->input_stream);

  ut_object_unref(self->input_stream);
  ut_object_weak_unref(&self->callback_object);
  ut_object_unref(self->literal_length_huffman_encoder);
  ut_object_unref(self->distance_huffman_encoder);
  ut_object_unref(self->dictionary);
  ut_object_unref(self->buffer);
}

static void ut_deflate_encoder_read(UtObject *object, UtObject *callback_object,
                                    UtInputStreamCallback callback) {
  UtDeflateEncoder *self = (UtDeflateEncoder *)object;
  assert(callback != NULL);
  assert(self->callback == NULL);
  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callback = callback;
  ut_input_stream_read(self->input_stream, object, read_cb);
}

static void ut_deflate_encoder_close(UtObject *object) {
  UtDeflateEncoder *self = (UtDeflateEncoder *)object;
  ut_input_stream_close(self->input_stream);
}

static UtInputStreamInterface input_stream_interface = {
    .read = ut_deflate_encoder_read, .close = ut_deflate_encoder_close};

static UtObjectInterface object_interface = {
    .type_name = "UtDeflateEncoder",
    .init = ut_deflate_encoder_init,
    .cleanup = ut_deflate_encoder_cleanup,
    .interfaces = {{&ut_input_stream_id, &input_stream_interface},
                   {NULL, NULL}}};

UtObject *ut_deflate_encoder_new(UtObject *input_stream) {
  return ut_deflate_encoder_new_with_window_size(32768, input_stream);
}

UtObject *ut_deflate_encoder_new_with_window_size(size_t window_size,
                                                  UtObject *input_stream) {
  assert(input_stream != NULL);
  UtObject *object = ut_object_new(sizeof(UtDeflateEncoder), &object_interface);
  UtDeflateEncoder *self = (UtDeflateEncoder *)object;
  self->input_stream = ut_object_ref(input_stream);
  self->window_size = window_size;
  return object;
}

size_t ut_deflate_encoder_get_window_size(UtObject *object) {
  assert(ut_object_is_deflate_encoder(object));
  UtDeflateEncoder *self = (UtDeflateEncoder *)object;
  return self->window_size;
}

bool ut_object_is_deflate_encoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
