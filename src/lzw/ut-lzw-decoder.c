#include <assert.h>

#include "ut-lzw-dictionary.h"
#include "ut.h"

typedef struct {
  UtObject object;

  UtObject *input_stream;
  UtObject *callback_object;
  UtInputStreamCallback callback;

  // Bytes that each code represents.
  UtObject *dictionary;

  // Length of next code to read.
  size_t code_length;

  // True if bits are packed least significant bit first, otherwise most
  // significant bit first.
  bool lsb_packing;

  // Current unprocessed bits being read from stream.
  uint32_t read_buffer;
  size_t read_buffer_bits;

  // Last code word received.
  uint16_t last_code;

  // Decoded data.
  UtObject *buffer;
} UtLzwDecoder;

// Report error in decoding.
static void error(UtLzwDecoder *self, const char *description) {
  UtObjectRef error = ut_lzw_error_new(description);
  if (self->callback_object != NULL) {
    self->callback(self->callback_object, error, true);
  }
}

// Update length of next code to read.
static void update_code_length(UtLzwDecoder *self) {
  size_t dictionary_length = ut_lzw_dictionary_get_length(self->dictionary);

  // We are one dictionary entry behind.
  if (self->last_code != ut_lzw_dictionary_get_clear_code(self->dictionary) &&
      !ut_lzw_dictionary_get_is_full(self->dictionary)) {
    dictionary_length++;
  }

  while (dictionary_length > (1 << self->code_length)) {
    self->code_length++;
  }
}

// Create a mask for the last [length] bits in a 16 bit word.
static uint16_t bit_mask(size_t length) { return 0xffff >> (16 - length); }

static size_t read_cb(UtObject *object, UtObject *data, bool complete) {
  UtLzwDecoder *self = (UtLzwDecoder *)object;

  size_t data_length = ut_list_get_length(data);

  size_t offset = 0;
  bool have_eoi = false;
  while (true) {
    // Read bytes from input so we have sufficient bits for a code word.
    update_code_length(self);
    while (self->read_buffer_bits < self->code_length && offset < data_length) {
      uint32_t b = ut_uint8_list_get_element(data, offset);
      if (self->lsb_packing) {
        self->read_buffer |= b << self->read_buffer_bits;
      } else {
        self->read_buffer = self->read_buffer << 8 | b;
      }
      self->read_buffer_bits += 8;
      offset++;
    }
    if (self->read_buffer_bits < self->code_length) {
      break;
    }

    // Extract next code.
    uint16_t code;
    if (self->lsb_packing) {
      code = self->read_buffer & bit_mask(self->code_length);
      self->read_buffer >>= self->code_length;
    } else {
      size_t unused_bits = self->read_buffer_bits - self->code_length;
      code = self->read_buffer >> unused_bits;
      self->read_buffer &= bit_mask(unused_bits);
    }
    self->read_buffer_bits -= self->code_length;

    // Process code.
    uint16_t clear_code = ut_lzw_dictionary_get_clear_code(self->dictionary);
    if (code == clear_code) {
      ut_lzw_dictionary_clear(self->dictionary);
      self->last_code = clear_code;
      self->code_length = 0;
    } else if (code == ut_lzw_dictionary_get_end_of_information_code(
                           self->dictionary)) {
      ut_input_stream_close(self->input_stream);
      have_eoi = true;
      break;
    } else {
      uint8_t first_symbol;
      size_t dictionary_length = ut_lzw_dictionary_get_length(self->dictionary);
      if (code < dictionary_length) {
        UtObject *entry = ut_lzw_dictionary_lookup(self->dictionary, code);
        ut_list_append_list(self->buffer, entry);
        first_symbol = ut_uint8_list_get_element(entry, 0);
      } else if (code == dictionary_length && self->last_code != clear_code) {
        UtObject *entry =
            ut_lzw_dictionary_lookup(self->dictionary, self->last_code);
        first_symbol = ut_uint8_list_get_element(entry, 0);
        ut_list_append_list(self->buffer, entry);
        ut_uint8_list_append(self->buffer, first_symbol);
      } else {
        error(self, "Invalid code received");
        return offset;
      }

      if (self->last_code != clear_code) {
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
    size_t n_used =
        self->callback_object != NULL
            ? self->callback(self->callback_object, self->buffer, have_eoi)
            : 0;
    ut_list_remove(self->buffer, 0, n_used);
  }

  return offset;
}

static void ut_lzw_decoder_init(UtObject *object) {
  UtLzwDecoder *self = (UtLzwDecoder *)object;
  self->buffer = ut_uint8_array_new();
}

static void ut_lzw_decoder_cleanup(UtObject *object) {
  UtLzwDecoder *self = (UtLzwDecoder *)object;

  ut_input_stream_close(self->input_stream);

  ut_object_unref(self->input_stream);
  ut_object_unref(self->dictionary);
  ut_object_unref(self->buffer);
}

static void ut_lzw_decoder_read(UtObject *object, UtObject *callback_object,
                                UtInputStreamCallback callback) {
  UtLzwDecoder *self = (UtLzwDecoder *)object;
  assert(callback != NULL);
  assert(self->callback == NULL);
  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callback = callback;
  ut_input_stream_read(self->input_stream, object, read_cb);
}

static void ut_lzw_decoder_close(UtObject *object) {
  UtLzwDecoder *self = (UtLzwDecoder *)object;
  ut_input_stream_close(self->input_stream);
}

static UtInputStreamInterface input_stream_interface = {
    .read = ut_lzw_decoder_read, .close = ut_lzw_decoder_close};

static UtObjectInterface object_interface = {
    .type_name = "UtLzwDecoder",
    .init = ut_lzw_decoder_init,
    .cleanup = ut_lzw_decoder_cleanup,
    .interfaces = {{&ut_input_stream_id, &input_stream_interface},
                   {NULL, NULL}}};

static UtObject *ut_lzw_decoder_new(size_t n_symbols,
                                    size_t max_dictionary_length,
                                    bool lsb_packing, UtObject *input_stream) {
  assert(input_stream != NULL);
  UtObject *object = ut_object_new(sizeof(UtLzwDecoder), &object_interface);
  UtLzwDecoder *self = (UtLzwDecoder *)object;
  self->dictionary = ut_lzw_dictionary_new(n_symbols, max_dictionary_length);
  self->last_code = ut_lzw_dictionary_get_clear_code(self->dictionary);
  self->input_stream = ut_object_ref(input_stream);
  self->lsb_packing = lsb_packing;
  return object;
}

UtObject *ut_lzw_decoder_new_lsb(size_t n_symbols, size_t max_dictionary_length,
                                 UtObject *input_stream) {
  return ut_lzw_decoder_new(n_symbols, max_dictionary_length, true,
                            input_stream);
}

UtObject *ut_lzw_decoder_new_msb(size_t n_symbols, size_t max_dictionary_length,
                                 UtObject *input_stream) {
  return ut_lzw_decoder_new(n_symbols, max_dictionary_length, false,
                            input_stream);
}

bool ut_object_is_lzw_decoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
