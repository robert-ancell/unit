#include <assert.h>

#include "ut-lzw-dictionary.h"
#include "ut.h"

typedef struct {
  UtObject object;

  UtObject *input_stream;
  UtObject *read_cancel;
  UtInputStreamCallback callback;
  void *user_data;
  UtObject *cancel;

  // Bytes that each code represents.
  UtObject *dictionary;

  // True if bits are packed least significant bit first, otherwise most
  // significant bit first.
  bool lsb_packing;

  // Current unprocessed bits being read from stream.
  uint16_t read_buffer;
  size_t read_buffer_bits;

  // Last code word received.
  uint16_t last_code;

  // Decoded data.
  UtObject *buffer;
} UtLzwDecoder;

// Report error in decoding.
static void error(UtLzwDecoder *self, const char *description) {
  UtObjectRef error = ut_lzw_error_new(description);
  self->callback(self->user_data, error, true);
}

// Create a mask for the last [length] bits in a 16 bit word.
static uint16_t bit_mask(size_t length) { return 0xffff >> (16 - length); }

static size_t read_cb(void *user_data, UtObject *data, bool complete) {
  UtLzwDecoder *self = user_data;

  if (ut_cancel_is_active(self->cancel)) {
    ut_cancel_activate(self->read_cancel);
    return 0;
  }

  size_t data_length = ut_list_get_length(data);

  size_t offset = 0;
  bool have_eoi = false;
  while (true) {
    // Read bytes from input so we have sufficient bits for a code word.
    size_t bit_length = ut_lzw_dictionary_get_code_length(self->dictionary);
    while (self->read_buffer_bits < bit_length && offset < data_length) {
      if (self->lsb_packing) {
        self->read_buffer |= ut_uint8_list_get_element(data, offset)
                             << self->read_buffer_bits;
      } else {
        self->read_buffer =
            self->read_buffer << 8 | ut_uint8_list_get_element(data, offset);
      }
      self->read_buffer_bits += 8;
      offset++;
    }
    if (self->read_buffer_bits < bit_length) {
      break;
    }

    // Extract next code.
    uint16_t code;
    if (self->lsb_packing) {
      code = self->read_buffer & bit_mask(bit_length);
      self->read_buffer >>= bit_length;
    } else {
      size_t unused_bits = self->read_buffer_bits - bit_length;
      code = self->read_buffer >> unused_bits;
      self->read_buffer &= bit_mask(unused_bits);
    }
    self->read_buffer_bits -= bit_length;

    // Process code.
    if (code == LZW_CLEAR_CODE) {
      ut_lzw_dictionary_clear(self->dictionary);
      self->last_code = LZW_CLEAR_CODE;
    } else if (code == LZW_END_OF_INFORMATION_CODE) {
      have_eoi = true;
      break;
    } else {
      size_t dictionary_length = ut_lzw_dictionary_get_length(self->dictionary);
      uint8_t first_symbol;
      if (code < dictionary_length) {
        UtObject *entry = ut_lzw_dictionary_lookup(self->dictionary, code);
        ut_list_append_list(self->buffer, entry);
        first_symbol = ut_uint8_list_get_element(entry, 0);
      } else if (code == dictionary_length &&
                 self->last_code != LZW_CLEAR_CODE) {
        UtObject *entry =
            ut_lzw_dictionary_lookup(self->dictionary, self->last_code);
        first_symbol = ut_uint8_list_get_element(entry, 0);
        ut_list_append_list(self->buffer, entry);
        ut_uint8_list_append(self->buffer, first_symbol);
      } else {
        error(self, "Invalid code received");
        return offset;
      }

      if (self->last_code != LZW_CLEAR_CODE) {
        ut_lzw_dictionary_append(self->dictionary, self->last_code,
                                 first_symbol);
      }
      self->last_code = code;
    }
  }

  if (complete && !have_eoi) {
    error(self, "Missing end of information");
    return offset;
  }

  size_t buffer_length = ut_list_get_length(self->buffer);
  if (buffer_length > 0 || have_eoi) {
    size_t n_used = self->callback(self->user_data, self->buffer, have_eoi);
    ut_list_remove(self->buffer, 0, n_used);
  }

  return offset;
}

static void ut_lzw_decoder_init(UtObject *object) {
  UtLzwDecoder *self = (UtLzwDecoder *)object;
  self->read_cancel = ut_cancel_new();
  self->buffer = ut_uint8_array_new();
  self->dictionary = ut_lzw_dictionary_new();
  self->last_code = LZW_CLEAR_CODE;
}

static void ut_lzw_decoder_cleanup(UtObject *object) {
  UtLzwDecoder *self = (UtLzwDecoder *)object;
  ut_cancel_activate(self->read_cancel);
  ut_object_unref(self->input_stream);
  ut_object_unref(self->read_cancel);
  ut_object_unref(self->cancel);
  ut_object_unref(self->dictionary);
  ut_object_unref(self->buffer);
}

static void ut_lzw_decoder_read(UtObject *object,
                                UtInputStreamCallback callback, void *user_data,
                                UtObject *cancel) {
  UtLzwDecoder *self = (UtLzwDecoder *)object;
  assert(callback != NULL);
  assert(self->callback == NULL);
  self->callback = callback;
  self->user_data = user_data;
  self->cancel = ut_object_ref(cancel);
  ut_input_stream_read(self->input_stream, read_cb, self, self->read_cancel);
}

static UtInputStreamInterface input_stream_interface = {
    .read = ut_lzw_decoder_read};

static UtObjectInterface object_interface = {
    .type_name = "UtLzwDecoder",
    .init = ut_lzw_decoder_init,
    .cleanup = ut_lzw_decoder_cleanup,
    .interfaces = {{&ut_input_stream_id, &input_stream_interface},
                   {NULL, NULL}}};

static UtObject *ut_lzw_decoder_new(bool lsb_packing, UtObject *input_stream) {
  assert(input_stream != NULL);
  UtObject *object = ut_object_new(sizeof(UtLzwDecoder), &object_interface);
  UtLzwDecoder *self = (UtLzwDecoder *)object;
  self->input_stream = ut_object_ref(input_stream);
  self->lsb_packing = lsb_packing;
  return object;
}

UtObject *ut_lzw_decoder_new_lsb(UtObject *input_stream) {
  return ut_lzw_decoder_new(true, input_stream);
}

UtObject *ut_lzw_decoder_new_msb(UtObject *input_stream) {
  return ut_lzw_decoder_new(false, input_stream);
}

bool ut_object_is_lzw_decoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
