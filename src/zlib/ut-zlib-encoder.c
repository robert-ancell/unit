#include <assert.h>
#include <stdint.h>

#include "ut.h"

#define METHOD_DEFLATE 8
#define COMPRESSION_DEFAULT 2

typedef struct {
  UtObject object;
  UtObject *input_stream;
  UtObject *read_cancel;
  UtInputStreamCallback callback;
  void *user_data;
  UtObject *cancel;

  UtZlibCompressionLevel compression_level;
  size_t window_size;

  // Checksum calculated of uncompressed data.
  uint32_t checksum;

  UtObject *deflate_input_stream;
  UtObject *deflate_encoder;

  // Encoded zlib data.
  bool written_header;
  UtObject *buffer;
} UtZlibEncoder;

static bool encode_window_size(size_t window_size, uint8_t *value) {
  if (window_size < 256 || window_size > 32768) {
    return false;
  }

  size_t exponent = 8;
  size_t w = window_size >> 8;
  while ((w & 0x1) == 0) {
    w >>= 1;
    exponent++;
  }

  if (w != 1) {
    return false;
  }

  if (value != NULL) {
    *value = exponent - 8;
  }

  return true;
}

static void write_header(UtZlibEncoder *self, uint8_t compression_method,
                         size_t window_size, uint8_t compression_level) {
  uint8_t window_size_value;
  assert(encode_window_size(window_size, &window_size_value));
  uint8_t cmf = window_size_value << 4 | compression_method;
  uint8_t flags = compression_level << 6;

  uint16_t header_check = (cmf << 8 | flags) % 31;
  if (header_check != 0) {
    flags |= 31 - header_check;
  }

  ut_uint8_list_append(self->buffer, cmf);
  ut_uint8_list_append(self->buffer, flags);
}

static uint32_t adler32(uint32_t checksum, uint8_t value) {
  uint32_t s1 = checksum & 0xffff;
  uint32_t s2 = checksum >> 16;
  s1 = (s1 + value) % 65521;
  s2 = (s2 + s1) % 65521;
  return s2 << 16 | s1;
}

static size_t deflate_read_cb(void *user_data, UtObject *data, bool complete) {
  UtZlibEncoder *self = user_data;
  ut_list_append_list(self->buffer, data);
  return ut_list_get_length(data);
}

static size_t read_cb(void *user_data, UtObject *data, bool complete) {
  UtZlibEncoder *self = user_data;

  if (ut_cancel_is_active(self->cancel)) {
    ut_cancel_activate(self->read_cancel);
    return 0;
  }

  if (!self->written_header) {
    write_header(self, METHOD_DEFLATE,
                 ut_deflate_encoder_get_window_size(self->deflate_encoder),
                 self->compression_level);
    self->written_header = true;
  }

  size_t n = ut_writable_input_stream_write(self->deflate_input_stream, data,
                                            complete);
  for (size_t i = 0; i < n; i++) {
    self->checksum =
        adler32(self->checksum, ut_uint8_list_get_element(data, i));
  }

  if (complete) {
    // Write the checksum.
    ut_uint8_list_append_uint32_be(self->buffer, self->checksum);
  }

  if (ut_list_get_length(self->buffer) > 0 || complete) {
    size_t n_used = self->callback(self->user_data, self->buffer, complete);
    ut_list_remove(self->buffer, 0, n_used);
  }

  return n;
}

static void ut_zlib_encoder_init(UtObject *object) {
  UtZlibEncoder *self = (UtZlibEncoder *)object;
  self->read_cancel = ut_cancel_new();
  self->deflate_input_stream = ut_writable_input_stream_new();
  self->buffer = ut_uint8_array_new();
}

static void ut_zlib_encoder_cleanup(UtObject *object) {
  UtZlibEncoder *self = (UtZlibEncoder *)object;
  ut_object_unref(self->input_stream);
  ut_cancel_activate(self->read_cancel);
  ut_object_unref(self->cancel);
  ut_object_unref(self->deflate_input_stream);
  ut_object_unref(self->deflate_encoder);
  ut_object_unref(self->buffer);
}

static void ut_zlib_encoder_read(UtObject *object,
                                 UtInputStreamCallback callback,
                                 void *user_data, UtObject *cancel) {
  UtZlibEncoder *self = (UtZlibEncoder *)object;
  assert(callback != NULL);
  assert(self->callback == NULL);
  self->callback = callback;
  self->user_data = user_data;
  self->cancel = ut_object_ref(cancel);
  self->checksum = 1;
  ut_input_stream_read(self->input_stream, read_cb, self, self->read_cancel);
}

static UtInputStreamInterface input_stream_interface = {
    .read = ut_zlib_encoder_read};

static UtObjectInterface object_interface = {
    .type_name = "UtZlibEncoder",
    .init = ut_zlib_encoder_init,
    .cleanup = ut_zlib_encoder_cleanup,
    .interfaces = {{&ut_input_stream_id, &input_stream_interface},
                   {NULL, NULL}}};

UtObject *ut_zlib_encoder_new(UtObject *input_stream) {
  return ut_zlib_encoder_new_full(UT_ZLIB_COMPRESSION_LEVEL_DEFAULT, 32768,
                                  input_stream);
}

UtObject *ut_zlib_encoder_new_full(UtZlibCompressionLevel compression_level,
                                   size_t window_size, UtObject *input_stream) {
  assert(input_stream != NULL);
  UtObject *object = ut_object_new(sizeof(UtZlibEncoder), &object_interface);
  UtZlibEncoder *self = (UtZlibEncoder *)object;

  if (compression_level > 3) {
    return ut_zlib_error_new("Invalid compression level");
  }
  if (!encode_window_size(window_size, NULL)) {
    return ut_zlib_error_new("Invalid window size");
  }
  self->compression_level = compression_level;
  self->window_size = window_size;

  self->input_stream = ut_object_ref(input_stream);

  self->deflate_encoder = ut_deflate_encoder_new_with_window_size(
      window_size, self->deflate_input_stream);
  ut_input_stream_read(self->deflate_encoder, deflate_read_cb, self,
                       self->read_cancel);

  return object;
}

bool ut_object_is_zlib_encoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
