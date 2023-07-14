#include <assert.h>
#include <stdint.h>

#include "ut.h"

typedef enum {
  DECODER_STATE_HEADER,
  DECODER_STATE_DICTIONARY,
  DECODER_STATE_COMPRESSED_DATA,
  DECODER_STATE_CHECKSUM,
  DECODER_STATE_DONE,
  DECODER_STATE_ERROR
} DecoderState;

typedef struct {
  UtObject object;
  UtObject *input_stream;
  UtObject *deflate_input_stream;
  UtInputStreamCallback callback;
  void *user_data;
  DecoderState state;
  uint16_t window_size;
  UtZlibCompressionLevel compression_level;
  uint32_t dictionary_checksum;
  uint32_t checksum;
  UtObject *deflate_decoder;
  UtObject *error;
} UtZlibDecoder;

static uint32_t adler32(uint32_t checksum, uint8_t value) {
  uint32_t s1 = checksum & 0xffff;
  uint32_t s2 = checksum >> 16;
  s1 = (s1 + value) % 65521;
  s2 = (s2 + s1) % 65521;
  return s2 << 16 | s1;
}

static void set_error(UtZlibDecoder *self, const char *description) {
  if (self->state == DECODER_STATE_ERROR) {
    return;
  }

  self->error = ut_zlib_error_new(description);
  self->state = DECODER_STATE_ERROR;

  self->callback(self->user_data, self->error, true);
}

static size_t deflate_read_cb(void *user_data, UtObject *data, bool complete) {
  UtZlibDecoder *self = user_data;

  if (ut_object_implements_error(data)) {
    ut_cstring_ref deflate_description = ut_error_get_description(data);
    ut_cstring_ref description = ut_cstring_new_printf(
        "Error decoding deflate data: %s", deflate_description);
    set_error(self, description);
    return 0;
  }

  size_t data_length = ut_list_get_length(data);
  size_t n_used = self->callback(self->user_data, data, complete);
  assert(n_used <= data_length);

  // Use all remaining data if complete, even if it wasn't used by the stream
  // consumer.
  if (complete) {
    n_used = data_length;
  }

  for (size_t i = 0; i < n_used; i++) {
    self->checksum =
        adler32(self->checksum, ut_uint8_list_get_element(data, i));
  }

  if (complete) {
    self->state = DECODER_STATE_CHECKSUM;
  }

  return n_used;
}

static size_t decode_header(UtZlibDecoder *self, UtObject *data) {
  size_t data_length = ut_list_get_length(data);

  if (data_length < 2) {
    return 0;
  }

  size_t offset = 0;
  uint8_t cmf = ut_uint8_list_get_element(data, offset++);
  uint8_t flags = ut_uint8_list_get_element(data, offset++);

  // Header checksum
  if ((cmf << 8 | flags) % 31 != 0) {
    set_error(self, "Zlib header checksum mismatch");
    return 0;
  }

  uint8_t compression_method = cmf & 0x0f;
  if (compression_method != 8) {
    set_error(self, "Unknown zlib compression method");
    return 0;
  }

  self->window_size = 1 << ((cmf >> 4) + 8);
  if (self->window_size > 32768) {
    set_error(self, "Invalid deflate window size");
    return 0;
  }
  self->compression_level = flags >> 6;
  bool has_preset_dictionary = (flags & 0x20) != 0;

  if (has_preset_dictionary) {
    if (data_length < offset + 4) {
      return 0;
    }
    self->dictionary_checksum = ut_uint8_list_get_uint32_be(data, offset);
    offset += 4;
  }

  self->checksum = 1;
  if (has_preset_dictionary) {
    self->state = DECODER_STATE_DICTIONARY;
  } else {
    self->state = DECODER_STATE_COMPRESSED_DATA;
  }

  return offset;
}

static size_t decode_dictionary(UtZlibDecoder *self, UtObject *data) {
  size_t data_length = ut_list_get_length(data);

  size_t offset = 0;
  while (self->checksum != self->dictionary_checksum && offset < data_length) {
    uint8_t value = ut_uint8_list_get_element(data, offset++);
    self->checksum = adler32(self->checksum, value);
  }

  if (self->checksum != self->dictionary_checksum) {
    return 0;
  }

  self->checksum = 1;
  self->state = DECODER_STATE_COMPRESSED_DATA;

  return offset;
}

static size_t decode_checksum(UtZlibDecoder *self, UtObject *data) {
  size_t data_length = ut_list_get_length(data);
  if (data_length < 4) {
    return 0;
  }

  uint32_t checksum = ut_uint8_list_get_uint32_be(data, 0);
  if (checksum != self->checksum) {
    set_error(self, "Zlib data checksum mismatch");
    return 0;
  }

  self->state = DECODER_STATE_DONE;
  return 4;
}

static size_t read_cb(void *user_data, UtObject *data, bool complete) {
  UtZlibDecoder *self = user_data;

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
    case DECODER_STATE_DICTIONARY:
      n_used = decode_dictionary(self, d);
      break;
    case DECODER_STATE_COMPRESSED_DATA:
      n_used = ut_writable_input_stream_write(self->deflate_input_stream, d,
                                              complete);
      break;
    case DECODER_STATE_CHECKSUM:
      n_used = decode_checksum(self, d);
      break;
    case DECODER_STATE_DONE:
    case DECODER_STATE_ERROR:
      return offset;
    default:
      assert(false);
    }

    offset += n_used;
    if (self->state == old_state && n_used == 0) {
      if (complete && self->state != DECODER_STATE_DONE) {
        set_error(self, "Incomplete zlib data");
      }
      return offset;
    }
  }
}

static void ut_zlib_decoder_init(UtObject *object) {
  UtZlibDecoder *self = (UtZlibDecoder *)object;
  self->state = DECODER_STATE_HEADER;
}

static void ut_zlib_decoder_cleanup(UtObject *object) {
  UtZlibDecoder *self = (UtZlibDecoder *)object;

  ut_input_stream_close(self->input_stream);
  ut_input_stream_close(self->deflate_decoder);

  ut_object_unref(self->input_stream);
  ut_object_unref(self->deflate_input_stream);
  ut_object_unref(self->deflate_decoder);
  ut_object_unref(self->error);
}

static void ut_zlib_decoder_read(UtObject *object,
                                 UtInputStreamCallback callback,
                                 void *user_data) {
  UtZlibDecoder *self = (UtZlibDecoder *)object;
  assert(callback != NULL);
  assert(self->callback == NULL);
  self->callback = callback;
  self->user_data = user_data;
  ut_input_stream_read(self->input_stream, read_cb, self);
}

static void ut_zlib_decoder_close(UtObject *object) {
  UtZlibDecoder *self = (UtZlibDecoder *)object;
  ut_input_stream_close(self->input_stream);
}

static UtInputStreamInterface input_stream_interface = {
    .read = ut_zlib_decoder_read, .close = ut_zlib_decoder_close};

static UtObjectInterface object_interface = {
    .type_name = "UtZlibDecoder",
    .init = ut_zlib_decoder_init,
    .cleanup = ut_zlib_decoder_cleanup,
    .interfaces = {{&ut_input_stream_id, &input_stream_interface},
                   {NULL, NULL}}};

UtObject *ut_zlib_decoder_new(UtObject *input_stream) {
  assert(input_stream != NULL);
  UtObject *object = ut_object_new(sizeof(UtZlibDecoder), &object_interface);
  UtZlibDecoder *self = (UtZlibDecoder *)object;
  self->input_stream = ut_object_ref(input_stream);
  self->deflate_input_stream = ut_writable_input_stream_new();
  self->deflate_decoder = ut_deflate_decoder_new(self->deflate_input_stream);
  ut_input_stream_read(self->deflate_decoder, deflate_read_cb, self);
  return object;
}

UtZlibCompressionLevel ut_zlib_decoder_get_compression_level(UtObject *object) {
  assert(ut_object_is_zlib_decoder(object));
  UtZlibDecoder *self = (UtZlibDecoder *)object;
  return self->compression_level;
}

bool ut_object_is_zlib_decoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
