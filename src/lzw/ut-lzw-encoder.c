#include <assert.h>

#include "ut-lzw-dictionary.h"
#include "ut.h"

typedef struct {
  UtObject object;

  // Data to be encoded.
  UtObject *input_stream;

  // Encoded data reader.
  UtObject *callback_object;
  UtInputStreamCallback callback;

  // Number of symbols.
  size_t n_symbols;

  // Bytes that each code represents.
  UtObject *dictionary;

  // Length of next code to write.
  size_t code_length;

  // True if bits are packed least significant bit first, otherwise most
  // significant bit first.
  bool lsb_packing;

  // Encoded data buffer.
  UtObject *buffer;

  // Number of unused bits on the end of [buffer].
  size_t unused_bits;
} UtLzwEncoder;

// Returns true if [data] matches [entry].
static bool matches(UtObject *entry, UtObject *data) {
  size_t entry_length = ut_list_get_length(entry);
  size_t data_length = ut_list_get_length(data);
  size_t max_length = data_length < entry_length ? data_length : entry_length;
  for (size_t i = 0; i < max_length; i++) {
    if (ut_uint8_list_get_element(entry, i) !=
        ut_uint8_list_get_element(data, i)) {
      return false;
    }
  }

  return true;
}

// Find a dictionary match for the given data.
static bool find_match(UtLzwEncoder *self, UtObject *data, bool complete,
                       size_t *match_length, uint16_t *code) {
  size_t data_length = ut_list_get_length(data);
  size_t dictionary_length = ut_lzw_dictionary_get_length(self->dictionary);
  UtObject *match = NULL;
  size_t match_index = 0;
  // FIXME: Reverse for efficiency (match longest first)
  for (size_t i = 0; i < dictionary_length; i++) {
    UtObject *entry = ut_lzw_dictionary_lookup(self->dictionary, i);
    size_t entry_length = ut_list_get_length(entry);

    // Existing match is longer.
    if (match != NULL && entry_length < ut_list_get_length(match)) {
      continue;
    }

    // Not enough data remaining for this entry.
    if (entry_length > data_length && complete) {
      continue;
    }

    if (matches(entry, data)) {
      match = entry;
      match_index = i;

      // May be able to match longer if have more data.
      if (entry_length > data_length && !complete) {
        return false;
      }
    }
  }

  *match_length = ut_list_get_length(match);
  *code = match_index;
  return true;
}

// Update length of next code to read.
static void update_code_length(UtLzwEncoder *self) {
  size_t dictionary_length = ut_lzw_dictionary_get_length(self->dictionary);
  while (dictionary_length > (1 << self->code_length)) {
    self->code_length++;
  }
}

// Create a mask for the last [length] bits in a 16 bit word.
static uint16_t bit_mask(size_t length) { return 0xffff >> (16 - length); }

// Write an LZW code to the output buffer.
static void write_code(UtLzwEncoder *self, uint16_t code) {
  update_code_length(self);

  // Write this code as fragments into an 8 bit buffer.
  size_t code_length = self->code_length;
  while (code_length > 0) {
    // Make more space.
    if (self->unused_bits == 0) {
      ut_uint8_list_append(self->buffer, 0x00);
      self->unused_bits = 8;
    }
    size_t buffer_length = ut_list_get_length(self->buffer);
    uint8_t *buffer_data = ut_uint8_list_get_writable_data(self->buffer);
    uint8_t value = buffer_data[buffer_length - 1];

    // Write as many bits as can fit into the available space.
    size_t fragment_bits =
        code_length > self->unused_bits ? self->unused_bits : code_length;
    if (self->lsb_packing) {
      uint16_t fragment = code & bit_mask(fragment_bits);
      value |= fragment << (8 - self->unused_bits);
      code >>= fragment_bits;
    } else {
      uint16_t fragment = code >> (code_length - fragment_bits);
      value |= fragment << (self->unused_bits - fragment_bits);
      code &= bit_mask(code_length - fragment_bits);
    }
    code_length -= fragment_bits;
    buffer_data[buffer_length - 1] = value;
    self->unused_bits -= fragment_bits;
  }
}

static size_t read_cb(UtObject *object, UtObject *data, bool complete) {
  UtLzwEncoder *self = (UtLzwEncoder *)object;

  size_t data_length = ut_list_get_length(data);
  size_t offset = 0;
  while (offset < data_length) {
    assert(ut_uint8_list_get_element(data, 0) < self->n_symbols);

    // Find the longest match in our dictionary.
    UtObjectRef d = ut_list_get_sublist(data, offset, data_length - offset);
    size_t match_length;
    uint16_t code;
    if (!find_match(self, d, complete, &match_length, &code)) {
      break;
    }
    write_code(self, code);
    offset += match_length;

    // New dictionary entry with next symbol appended to just used match.
    // Note on the last code an entry is written using 0 to ensure the following
    // code is the correct length.
    uint8_t b =
        offset < data_length ? ut_uint8_list_get_element(data, offset) : 0;
    ut_lzw_dictionary_append(self->dictionary, code, b);

    // Reset dictionary when it's full.
    // FIXME: Ideally would look ahead and determine if current dictionary is
    // sufficient.
    if (ut_lzw_dictionary_get_is_full(self->dictionary)) {
      write_code(self, ut_lzw_dictionary_get_clear_code(self->dictionary));

      ut_lzw_dictionary_clear(self->dictionary);
      self->code_length = 0;
    }
  }

  if (complete) {
    write_code(self,
               ut_lzw_dictionary_get_end_of_information_code(self->dictionary));
  }

  size_t buffer_length = ut_list_get_length(self->buffer);
  size_t buffer_length_bits = buffer_length * 8 - self->unused_bits;
  if (buffer_length_bits > 8 || complete) {
    UtObjectRef b =
        self->unused_bits == 0 || complete
            ? ut_object_ref(self->buffer)
            : ut_list_get_sublist(self->buffer, 0, buffer_length - 1);
    size_t n = self->callback_object != NULL
                   ? self->callback(self->callback_object, b, complete)
                   : 0;
    ut_list_remove(self->buffer, 0, n);
  }

  return offset;
}

static void ut_lzw_encoder_init(UtObject *object) {
  UtLzwEncoder *self = (UtLzwEncoder *)object;
  self->buffer = ut_uint8_list_new();
}

static void ut_lzw_encoder_cleanup(UtObject *object) {
  UtLzwEncoder *self = (UtLzwEncoder *)object;

  ut_input_stream_close(self->input_stream);

  ut_object_unref(self->input_stream);
  ut_object_weak_unref(&self->callback_object);
  ut_object_unref(self->dictionary);
  ut_object_unref(self->buffer);
}

static void ut_lzw_encoder_read(UtObject *object, UtObject *callback_object,
                                UtInputStreamCallback callback) {
  UtLzwEncoder *self = (UtLzwEncoder *)object;

  assert(callback != NULL);
  assert(self->callback == NULL);

  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callback = callback;

  // Always starts with a clear code.
  write_code(self, ut_lzw_dictionary_get_clear_code(self->dictionary));

  ut_input_stream_read(self->input_stream, object, read_cb);
}

static void ut_lzw_encoder_close(UtObject *object) {
  UtLzwEncoder *self = (UtLzwEncoder *)object;
  ut_input_stream_close(self->input_stream);
}

static UtInputStreamInterface input_stream_interface = {
    .read = ut_lzw_encoder_read, .close = ut_lzw_encoder_close};

static UtObjectInterface object_interface = {
    .type_name = "UtLzwEncoder",
    .init = ut_lzw_encoder_init,
    .cleanup = ut_lzw_encoder_cleanup,
    .interfaces = {{&ut_input_stream_id, &input_stream_interface},
                   {NULL, NULL}}};

static UtObject *ut_lzw_encoder_new(size_t n_symbols,
                                    size_t max_dictionary_length,
                                    bool lsb_packing, UtObject *input_stream) {
  assert(input_stream != NULL);
  UtObject *object = ut_object_new(sizeof(UtLzwEncoder), &object_interface);
  UtLzwEncoder *self = (UtLzwEncoder *)object;
  self->n_symbols = n_symbols;
  self->dictionary = ut_lzw_dictionary_new(n_symbols, max_dictionary_length);
  self->input_stream = ut_object_ref(input_stream);
  self->lsb_packing = lsb_packing;
  return object;
}

UtObject *ut_lzw_encoder_new_lsb(size_t n_symbols, size_t max_dictionary_length,
                                 UtObject *input_stream) {
  return ut_lzw_encoder_new(n_symbols, max_dictionary_length, true,
                            input_stream);
}

UtObject *ut_lzw_encoder_new_msb(size_t n_symbols, size_t max_dictionary_length,
                                 UtObject *input_stream) {
  return ut_lzw_encoder_new(n_symbols, max_dictionary_length, false,
                            input_stream);
}

bool ut_object_is_lzw_encoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
