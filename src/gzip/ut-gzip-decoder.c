#include <assert.h>
#include <stdint.h>

#include "ut.h"

// https://www.ietf.org/rfc/rfc1952.txt

typedef enum {
  DECODER_STATE_MEMBER_HEADER,
  DECODER_STATE_MEMBER_DATA,
  DECODER_STATE_MEMBER_TRAILER,
  DECODER_STATE_DONE,
  DECODER_STATE_ERROR
} DecoderState;

typedef struct {
  UtObject object;
  UtObject *input_stream;
  UtObject *deflate_input_stream;
  UtObject *callback_object;
  UtInputStreamCallback callback;

  DecoderState state;

  UtObject *deflate_decoder;

  // CRC of decoded data received.
  uint32_t crc;

  // Number of bytes of decoded data received.
  size_t data_length;

  // Error that occurred during decoding.
  UtObject *error;
} UtGzipDecoder;

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

static uint32_t crc32(uint32_t checksum, uint8_t value) {
  uint32_t c = checksum ^ 0xffffffff;
  c = crc_table[(c ^ value) & 0xff] ^ (c >> 8);
  return c ^ 0xffffffff;
}

static void set_error(UtGzipDecoder *self, const char *description) {
  if (self->state == DECODER_STATE_ERROR) {
    return;
  }

  self->error = ut_png_error_new(description);
  self->state = DECODER_STATE_ERROR;

  if (self->callback_object != NULL) {
    self->callback(self->callback_object, self->error, true);
  }
}

static void set_error_take(UtGzipDecoder *self, char *description) {
  set_error(self, description);
  free(description);
}

static size_t deflate_read_cb(UtObject *object, UtObject *data, bool complete) {
  UtGzipDecoder *self = (UtGzipDecoder *)object;

  if (ut_object_implements_error(data)) {
    set_error_take(self,
                   ut_cstring_new_printf("Error decoding deflate data: %s",
                                         ut_error_get_description(data)));
    return 0;
  }

  size_t data_length = ut_list_get_length(data);
  size_t n_used = self->callback_object != NULL
                      ? self->callback(self->callback_object, data, complete)
                      : 0;
  assert(n_used <= data_length);

  // Use all remaining data if complete, even if it wasn't used by the stream
  // consumer.
  if (complete) {
    n_used = data_length;
  }

  for (size_t i = 0; i < n_used; i++) {
    self->crc = crc32(self->crc, ut_uint8_list_get_element(data, i));
  }
  self->data_length += n_used;

  if (complete) {
    self->state = DECODER_STATE_MEMBER_TRAILER;
  }

  return n_used;
}

static char *read_string(UtObject *data, size_t *offset) {
  size_t data_length = ut_list_get_length(data);
  UtObjectRef value = ut_uint8_list_new();
  for (size_t i = *offset; i < data_length; i++) {
    uint8_t c = ut_uint8_list_get_element(data, i);
    ut_uint8_list_append(value, c);
    if (c == 0) {
      *offset = i + 1;
      return (char *)ut_uint8_list_take_data(value);
    }
  }

  return NULL;
}

static size_t decode_member_header(UtGzipDecoder *self, UtObject *data,
                                   bool complete) {
  size_t data_length = ut_list_get_length(data);

  if (data_length == 0 && complete) {
    self->state = DECODER_STATE_DONE;
    return 0;
  }

  if (data_length < 10) {
    return 0;
  }

  size_t offset = 0;
  uint8_t id1 = ut_uint8_list_get_element(data, offset++);
  uint8_t id2 = ut_uint8_list_get_element(data, offset++);
  uint8_t compression_method = ut_uint8_list_get_element(data, offset++);
  uint8_t flags = ut_uint8_list_get_element(data, offset++);
  /*uint32_t modification_time = */
  ut_uint8_list_get_uint32_le(data, offset);
  offset += 4;
  /*uint8_t extra_flags = */
  ut_uint8_list_get_element(data, offset++);
  /*uint8_t operating_system = */
  ut_uint8_list_get_element(data, offset++);

  if (id1 != 31 || id2 != 139) {
    set_error(self, "Invalid gzip ID");
    return 0;
  }

  if (compression_method != 8) {
    set_error(self, "Unsupported gzip compression method");
    return 0;
  }

  bool has_crc = (flags & 0x02) != 0;
  bool has_extra = (flags & 0x04) != 0;
  bool has_file_name = (flags & 0x08) != 0;
  bool has_file_comment = (flags & 0x10) != 0;

  if (has_extra) {
    if (data_length < offset + 2) {
      return 0;
    }
    uint16_t xlen = ut_uint8_list_get_uint16_le(data, offset);
    offset += 2;
    if (data_length < offset + xlen) {
      return 0;
    }
    offset += xlen;
  }

  if (has_file_name) {
    ut_cstring_ref file_name = read_string(data, &offset);
    if (file_name == NULL) {
      return 0;
    }
  }

  if (has_file_comment) {
    ut_cstring_ref file_comment = read_string(data, &offset);
    if (file_comment == NULL) {
      return 0;
    }
  }

  if (has_crc) {
    uint32_t crc = 0;
    for (size_t i = 0; i < offset; i++) {
      crc = crc32(crc, ut_uint8_list_get_element(data, i));
    }

    if (data_length < offset + 2) {
      return 0;
    }
    uint16_t header_crc = ut_uint8_list_get_uint16_le(data, offset);
    offset += 2;

    if (header_crc != (crc & 0xffff)) {
      set_error(self, "Gzip header CRC mismatch");
      return 0;
    }
  }

  self->state = DECODER_STATE_MEMBER_DATA;
  return offset;
}

static size_t decode_member_trailer(UtGzipDecoder *self, UtObject *data) {
  size_t data_length = ut_list_get_length(data);
  if (data_length < 8) {
    return 0;
  }

  uint32_t input_data_crc = ut_uint8_list_get_uint32_le(data, 0);
  uint32_t input_data_length = ut_uint8_list_get_uint32_le(data, 4);

  if (self->crc != input_data_crc) {
    set_error(self, "gzip data CRC mismatch");
    return 0;
  }
  if ((self->data_length & 0xffffffff) != input_data_length) {
    set_error(self, "gzip data length mismatch");
    return 0;
  }

  self->state = DECODER_STATE_MEMBER_HEADER;

  return 8;
}

static size_t read_cb(UtObject *object, UtObject *data, bool complete) {
  UtGzipDecoder *self = (UtGzipDecoder *)object;

  size_t data_length = ut_list_get_length(data);
  size_t offset = 0;
  while (true) {
    size_t n_used;
    UtObjectRef d = ut_list_get_sublist(data, offset, data_length - offset);
    DecoderState old_state = self->state;
    switch (self->state) {
    case DECODER_STATE_MEMBER_HEADER:
      n_used = decode_member_header(self, d, complete);
      break;
    case DECODER_STATE_MEMBER_DATA:
      n_used = ut_writable_input_stream_write(self->deflate_input_stream, d,
                                              complete);
      break;
    case DECODER_STATE_MEMBER_TRAILER:
      n_used = decode_member_trailer(self, d);
      break;
    case DECODER_STATE_DONE:
    case DECODER_STATE_ERROR:
      return offset;
    }

    offset += n_used;
    if (self->state == old_state && n_used == 0) {
      if (complete && self->state != DECODER_STATE_DONE) {
        set_error(self, "Incomplete gzip data");
      }
      return offset;
    }
  }
}

static void ut_gzip_decoder_init(UtObject *object) {
  UtGzipDecoder *self = (UtGzipDecoder *)object;
  self->state = DECODER_STATE_MEMBER_HEADER;
}

static void ut_gzip_decoder_cleanup(UtObject *object) {
  UtGzipDecoder *self = (UtGzipDecoder *)object;

  ut_input_stream_close(self->deflate_decoder);
  ut_input_stream_close(self->input_stream);

  ut_object_unref(self->input_stream);
  ut_object_unref(self->deflate_input_stream);
  ut_object_weak_unref(&self->callback_object);
  ut_object_unref(self->deflate_decoder);
  ut_object_unref(self->error);
}

static void ut_gzip_decoder_read(UtObject *object, UtObject *callback_object,
                                 UtInputStreamCallback callback) {
  UtGzipDecoder *self = (UtGzipDecoder *)object;
  assert(callback != NULL);
  assert(self->callback == NULL);
  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callback = callback;
  ut_input_stream_read(self->input_stream, object, read_cb);
}

static void ut_gzip_decoder_close(UtObject *object) {
  UtGzipDecoder *self = (UtGzipDecoder *)object;
  ut_input_stream_close(self->input_stream);
}

static UtInputStreamInterface input_stream_interface = {
    .read = ut_gzip_decoder_read, .close = ut_gzip_decoder_close};

static UtObjectInterface object_interface = {
    .type_name = "UtGzipDecoder",
    .init = ut_gzip_decoder_init,
    .cleanup = ut_gzip_decoder_cleanup,
    .interfaces = {{&ut_input_stream_id, &input_stream_interface},
                   {NULL, NULL}}};

UtObject *ut_gzip_decoder_new(UtObject *input_stream) {
  assert(input_stream != NULL);
  UtObject *object = ut_object_new(sizeof(UtGzipDecoder), &object_interface);
  UtGzipDecoder *self = (UtGzipDecoder *)object;
  self->input_stream = ut_object_ref(input_stream);
  self->deflate_input_stream = ut_writable_input_stream_new();
  self->deflate_decoder = ut_deflate_decoder_new(self->deflate_input_stream);
  ut_input_stream_read(self->deflate_decoder, object, deflate_read_cb);
  return object;
}

bool ut_object_is_gzip_decoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
