#include <assert.h>

#include "ut.h"

// Code used to clear dictionary.
#define CLEAR_CODE 256

// Code used for end of stream.
#define END_OF_INFORMATION_CODE 257

typedef struct {
  UtObject object;

  // Data to be encoded.
  UtObject *input_stream;
  UtObject *read_cancel;

  // Encoded data reader.
  UtInputStreamCallback callback;
  void *user_data;
  UtObject *cancel;

  // Bytes that each code represents.
  UtObject *dictionary;
  size_t bit_length;

  // True if bits are packed least significant bit first, otherwise most significant bit first.
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
  size_t dictionary_length = ut_list_get_length(self->dictionary);
  UtObject *match = NULL;
  size_t match_index = 0;
  // FIXME: Reverse for efficiency (match longest first)
  for (size_t i = 0; i < dictionary_length; i++) {
    UtObject *entry = ut_object_list_get_element(self->dictionary, i);
    size_t entry_length = ut_list_get_length(entry);

    // Existing match is longer.
    if (match != NULL && entry_length < ut_list_get_length(match)) {
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

static void add_dictionary_entry(UtLzwEncoder *self, uint16_t code, uint8_t b) {
  UtObject *entry = ut_object_list_get_element(self->dictionary, code);
  UtObjectRef new_entry = ut_list_copy(entry);
  ut_uint8_list_append(new_entry, b);
  ut_list_append(self->dictionary, new_entry);

  // Need to use more bits as the dictionary gets bigger.
  if (ut_list_get_length(self->dictionary) == 1 << self->bit_length) {
    self->bit_length++;
  }
}

// Create a mask for the last [length] bits in a 16 bit word.
static uint16_t bit_mask(size_t length) { return 0xffff >> (16 - length); }

// Write an LZW code to the output buffer.
static void write_code(UtLzwEncoder *self, uint16_t code) {
  // If exceeded maximum code lenth then reset dictionary.
  if (self->bit_length == 13) {
    // FIXME
  }

  // Write this code as fragments into an 8 bit buffer.
  size_t code_bits = self->bit_length;
  while (code_bits > 0) {
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
        code_bits > self->unused_bits ? self->unused_bits : code_bits;
    if (self->lsb_packing) {
      uint16_t fragment = code & bit_mask(fragment_bits);
      value |= fragment << (8 - self->unused_bits);
      code >>= fragment_bits;
    } else {
      uint16_t fragment = code >> (code_bits - fragment_bits);
      value |= fragment << (self->unused_bits - fragment_bits);
      code &= bit_mask(code_bits - fragment_bits);
    }
    code_bits -= fragment_bits;
    buffer_data[buffer_length - 1] = value;
    self->unused_bits -= fragment_bits;
  }
}

static size_t read_cb(void *user_data, UtObject *data, bool complete) {
  UtLzwEncoder *self = user_data;

  if (ut_cancel_is_active(self->cancel)) {
    ut_cancel_activate(self->read_cancel);
    return 0;
  }

  size_t data_length = ut_list_get_length(data);
  size_t offset = 0;
  while (offset < data_length) {
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
    if (offset < data_length) {
      uint8_t b = ut_uint8_list_get_element(data, offset);
      add_dictionary_entry(self, code, b);
    }
  }

  if (complete) {
    write_code(self, END_OF_INFORMATION_CODE);
  }

  size_t buffer_length = ut_list_get_length(self->buffer);
  size_t buffer_length_bits = buffer_length * 8 - self->unused_bits;
  if (buffer_length_bits > 8 || complete) {
    UtObjectRef b =
        self->unused_bits == 0 || complete
            ? ut_object_ref(self->buffer)
            : ut_list_get_sublist(self->buffer, 0, buffer_length - 1);
    size_t n = self->callback(self->user_data, b, complete);
    ut_list_remove(self->buffer, 0, n);
  }

  return offset;
}

static void ut_lzw_encoder_init(UtObject *object) {
  UtLzwEncoder *self = (UtLzwEncoder *)object;
  self->read_cancel = ut_cancel_new();
  self->buffer = ut_uint8_list_new();
  self->unused_bits = 0;

  // Initalize dictionary with 0-255
  self->dictionary = ut_object_list_new();
  for (size_t i = 0; i < 256; i++) {
    UtObjectRef entry = ut_uint8_list_new_from_elements(1, i);
    ut_list_append(self->dictionary, entry);
  }
  // Clear
  UtObjectRef clear_entry = ut_uint8_list_new();
  ut_list_append(self->dictionary, clear_entry);
  // End of Information
  UtObjectRef eoi_entry = ut_uint8_list_new();
  ut_list_append(self->dictionary, eoi_entry);

  // We start with 9 bits to fit all 258 codes.
  self->bit_length = 9;
}

static void ut_lzw_encoder_cleanup(UtObject *object) {
  UtLzwEncoder *self = (UtLzwEncoder *)object;
  ut_cancel_activate(self->read_cancel);
  ut_object_unref(self->input_stream);
  ut_object_unref(self->read_cancel);
  ut_object_unref(self->cancel);
  ut_object_unref(self->dictionary);
  ut_object_unref(self->buffer);
}

static void ut_lzw_encoder_read(UtObject *object,
                                UtInputStreamCallback callback, void *user_data,
                                UtObject *cancel) {
  UtLzwEncoder *self = (UtLzwEncoder *)object;

  assert(callback != NULL);
  assert(self->callback == NULL);

  self->callback = callback;
  self->user_data = user_data;
  self->cancel = ut_object_ref(cancel);

  // Always starts with a clear code.
  write_code(self, CLEAR_CODE);

  ut_input_stream_read(self->input_stream, read_cb, self, self->read_cancel);
}

static UtInputStreamInterface input_stream_interface = {
    .read = ut_lzw_encoder_read};

static UtObjectInterface object_interface = {
    .type_name = "UtLzwEncoder",
    .init = ut_lzw_encoder_init,
    .cleanup = ut_lzw_encoder_cleanup,
    .interfaces = {{&ut_input_stream_id, &input_stream_interface},
                   {NULL, NULL}}};

static UtObject *ut_lzw_encoder_new(bool lsb_packing, UtObject *input_stream) {
  assert(input_stream != NULL);
  UtObject *object = ut_object_new(sizeof(UtLzwEncoder), &object_interface);
  UtLzwEncoder *self = (UtLzwEncoder *)object;
  self->input_stream = ut_object_ref(input_stream);
  self->lsb_packing = lsb_packing;
  return object;
}

UtObject *ut_lzw_encoder_new_lsb(UtObject *input_stream) {
  return ut_lzw_encoder_new(true, input_stream);
}

UtObject *ut_lzw_encoder_new_msb(UtObject *input_stream) {
  return ut_lzw_encoder_new(false, input_stream);
}

bool ut_object_is_lzw_encoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
