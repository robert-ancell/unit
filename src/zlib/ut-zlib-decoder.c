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
  UtObject *read_cancel;
  UtInputStreamCallback callback;
  void *user_data;
  UtObject *cancel;
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
}

static size_t deflate_read_cb(void *user_data, UtObject *data, bool complete) {
  UtZlibDecoder *self = user_data;

  if (ut_cancel_is_active(self->cancel)) {
    ut_cancel_activate(self->read_cancel);
    return 0;
  }

  if (ut_object_implements_error(data)) {
    ut_cstring_ref deflate_description = ut_error_get_description(data);
    ut_cstring_ref description =
        ut_cstring_new_printf("Error decoding deflate data: %s", description);
    set_error(self, description);
    return 0;
  }

  size_t total_used = 0;
  size_t data_length = ut_list_get_length(data);

  do {
    size_t remaining_data_length = data_length - total_used;
    UtObjectRef remaining_data =
        total_used == 0
            ? ut_object_ref(data)
            : ut_list_get_sublist(data, total_used, remaining_data_length);
    size_t n_used = self->callback(self->user_data, remaining_data, complete);
    assert(n_used <= remaining_data_length);
    total_used += n_used;

    for (size_t i = 0; i < n_used; i++) {
      self->checksum =
          adler32(self->checksum, ut_uint8_list_get_element(remaining_data, i));
    }
  } while (!ut_cancel_is_active(self->cancel) && total_used < data_length);
  assert(total_used <= data_length);

  if (ut_cancel_is_active(self->cancel)) {
    ut_cancel_activate(self->read_cancel);
    return 0;
  }

  if (complete) {
    self->state = DECODER_STATE_CHECKSUM;
  }

  return total_used;
}

static bool read_header(UtZlibDecoder *self, UtObject *data, size_t *offset,
                        bool complete) {
  size_t data_length = ut_list_get_length(data);

  size_t header_start = *offset;
  size_t header_end = header_start + 2;
  if (data_length < header_end) {
    return false;
  }

  uint8_t cmf = ut_uint8_list_get_element(data, header_start + 0);
  uint8_t flags = ut_uint8_list_get_element(data, header_start + 1);

  // Header checksum
  if ((cmf << 8 | flags) % 31 != 0) {
    set_error(self, "Zlib header checksum mismatch");
    return true;
  }

  uint8_t compression_method = cmf & 0x0f;
  if (compression_method != 8) {
    set_error(self, "Unknown zlib compression method");
    return true;
  }

  self->window_size = 1 << ((cmf >> 4) + 8);
  if (self->window_size > 32768) {
    set_error(self, "Invalid deflate window size");
    return true;
  }
  self->compression_level = flags >> 6;
  bool has_preset_dictionary = (flags & 0x20) != 0;

  if (has_preset_dictionary) {
    self->dictionary_checksum =
        ut_uint8_list_get_uint32_be(data, header_start + 2);
    header_end += 4;
    if (data_length < header_end) {
      return false;
    }
  }

  *offset = header_end;
  self->checksum = 1;
  if (has_preset_dictionary) {
    self->state = DECODER_STATE_DICTIONARY;
  } else {
    self->state = DECODER_STATE_COMPRESSED_DATA;
  }

  return true;
}

static bool read_dictionary(UtZlibDecoder *self, UtObject *data, size_t *offset,
                            bool complete) {
  size_t data_length = ut_list_get_length(data);

  size_t dictionary_start = *offset;
  size_t dictionary_end = dictionary_start;
  while (self->checksum != self->dictionary_checksum &&
         dictionary_end < data_length) {
    uint8_t value = ut_uint8_list_get_element(data, dictionary_end);
    self->checksum = adler32(self->checksum, value);
    dictionary_end++;
  }

  *offset = dictionary_end;
  if (self->checksum == self->dictionary_checksum) {
    self->checksum = 1;
    self->state = DECODER_STATE_COMPRESSED_DATA;
    return true;
  } else if (complete) {
    set_error(self, "Zlib dictionary checksum mismatch");
    return true;
  }

  return dictionary_end != dictionary_start;
}

static bool read_compressed_data(UtZlibDecoder *self, UtObject *data,
                                 size_t *offset, bool complete) {
  size_t data_length = ut_list_get_length(data);

  UtObjectRef deflate_data =
      ut_list_get_sublist(data, *offset, data_length - *offset);
  size_t deflate_offset = ut_writable_input_stream_write(
      self->deflate_input_stream, deflate_data, complete);
  if (deflate_offset == 0) {
    return false;
  }

  *offset += deflate_offset;
  return true;
}

static bool read_checksum(UtZlibDecoder *self, UtObject *data, size_t *offset,
                          bool complete) {
  size_t data_length = ut_list_get_length(data);
  size_t checksum_start = *offset;
  size_t checksum_end = checksum_start + 4;
  if (data_length < checksum_end) {
    return false;
  }

  uint32_t checksum = ut_uint8_list_get_uint32_be(data, checksum_start);
  if (checksum != self->checksum) {
    set_error(self, "Zlib data checksum mismatch");
    return true;
  }

  *offset += 4;

  self->state = DECODER_STATE_DONE;
  return true;
}

static size_t read_cb(void *user_data, UtObject *data, bool complete) {
  UtZlibDecoder *self = user_data;

  size_t offset = 0;
  bool decoding = true;
  while (decoding) {
    if (ut_cancel_is_active(self->cancel)) {
      ut_cancel_activate(self->read_cancel);
      break;
    }

    switch (self->state) {
    case DECODER_STATE_HEADER:
      decoding = read_header(self, data, &offset, complete);
      break;
    case DECODER_STATE_DICTIONARY:
      decoding = read_dictionary(self, data, &offset, complete);
      break;
    case DECODER_STATE_COMPRESSED_DATA:
      decoding = read_compressed_data(self, data, &offset, complete);
      break;
    case DECODER_STATE_CHECKSUM:
      decoding = read_checksum(self, data, &offset, complete);
      break;
    case DECODER_STATE_DONE:
      ut_cancel_activate(self->read_cancel);
      decoding = false;
      break;
    case DECODER_STATE_ERROR:
      ut_cancel_activate(self->read_cancel);
      if (!ut_cancel_is_active(self->cancel)) {
        self->callback(self->user_data, self->error, true);
      }
      decoding = false;
      break;
    }
  }

  return offset;
}

static void ut_zlib_decoder_init(UtObject *object) {
  UtZlibDecoder *self = (UtZlibDecoder *)object;
  self->read_cancel = ut_cancel_new();
  self->state = DECODER_STATE_HEADER;
}

static void ut_zlib_decoder_cleanup(UtObject *object) {
  UtZlibDecoder *self = (UtZlibDecoder *)object;
  ut_cancel_activate(self->read_cancel);
  ut_object_unref(self->input_stream);
  ut_object_unref(self->deflate_input_stream);
  ut_object_unref(self->read_cancel);
  ut_object_unref(self->cancel);
  ut_object_unref(self->deflate_decoder);
  ut_object_unref(self->error);
}

static void ut_zlib_decoder_read(UtObject *object,
                                 UtInputStreamCallback callback,
                                 void *user_data, UtObject *cancel) {
  UtZlibDecoder *self = (UtZlibDecoder *)object;
  assert(callback != NULL);
  assert(self->callback == NULL);
  self->callback = callback;
  self->user_data = user_data;
  self->cancel = ut_object_ref(cancel);
  ut_input_stream_read(self->input_stream, read_cb, self, self->read_cancel);
}

static UtInputStreamInterface input_stream_interface = {
    .read = ut_zlib_decoder_read};

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
  ut_input_stream_read(self->deflate_decoder, deflate_read_cb, self,
                       self->read_cancel);
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
