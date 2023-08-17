#include <assert.h>
#include <string.h>

#include "ut.h"

typedef struct {
  UtObject object;

  // The data packed into 8 bit bytes.
  UtObject *data;

  // The total number of bits in [data].
  size_t bit_count;

  // True if new bits are added to the little end of a byte.
  bool little_endian;
} UtBitList;

static bool get_element(UtBitList *self, size_t index) {
  size_t byte_index = index / 8;
  size_t bit_index = index % 8;
  uint8_t mask;
  if (self->little_endian) {
    mask = 1 << bit_index;
  } else {
    mask = 1 << (7 - bit_index);
  }
  uint8_t byte = ut_uint8_list_get_element(self->data, byte_index);
  return (byte & mask) != 0;
}

static void append(UtBitList *self, uint8_t data, size_t bit_count) {
  size_t data_length = ut_list_get_length(self->data);
  size_t unused_count = (data_length * 8) - self->bit_count;

  // FIXME: Optimize single bit case

  if (unused_count == 0) {
    if (self->little_endian) {
      ut_uint8_list_append(self->data, data);
    } else {
      ut_uint8_list_append(self->data, data << (8 - bit_count));
    }
  } else {
    // Split data across bytes.
    size_t bit_count1 = unused_count < bit_count ? unused_count : bit_count;
    size_t bit_count2 = bit_count - bit_count1;
    uint8_t data1 = data >> bit_count2;
    uint8_t data2 = data & ((1 << bit_count2) - 1);

    uint8_t *d = ut_uint8_list_get_writable_data(self->data);
    assert(d != NULL);

    if (self->little_endian) {
      d[data_length - 1] |= data1 << (8 - unused_count);
    } else {
      d[data_length - 1] |= data1 << (unused_count - bit_count1);
    }

    if (bit_count2 > 0) {
      if (self->little_endian) {
        ut_uint8_list_append(self->data, data2);
      } else {
        ut_uint8_list_append(self->data, data2 << (8 - bit_count2));
      }
    }
  }

  self->bit_count += bit_count;
}

static bool append_bin_string(UtBitList *self, const char *bin) {
  size_t bin_length = strlen(bin);
  for (size_t i = 0; i < bin_length; i++) {
    uint8_t value;
    switch (bin[i]) {
    case '0':
      value = 0;
      break;
    case '1':
      value = 1;
      break;
    default:
      return false;
    }
    // FIXME: Do in 8 bit blocks
    append(self, value, 1);
  }

  return true;
}

static char *ut_bit_list_to_string(UtObject *object) {
  UtBitList *self = (UtBitList *)object;
  UtObjectRef string = ut_string_new("<bool>[");
  for (size_t i = 0; i < self->bit_count; i++) {
    if (i != 0) {
      ut_string_append(string, ", ");
    }
    ut_string_append(string, get_element(self, i) ? "true" : "false");
  }
  ut_string_append(string, "]");

  return ut_string_take_text(string);
}

static void ut_bit_list_cleanup(UtObject *object) {
  UtBitList *self = (UtBitList *)object;
  ut_object_unref(self->data);
}

static UtObjectInterface object_interface = {.type_name = "UtBitList",
                                             .to_string = ut_bit_list_to_string,
                                             .cleanup = ut_bit_list_cleanup};

static UtObject *bit_list_new(UtObject *data, size_t bit_count,
                              bool little_endian) {
  UtObject *object = ut_object_new(sizeof(UtBitList), &object_interface);
  UtBitList *self = (UtBitList *)object;
  if (data != NULL) {
    assert(ut_list_get_length(data) * 8 >= bit_count);
    self->data = ut_object_ref(data);
  } else {
    self->data = ut_uint8_list_new();
  }
  self->bit_count = bit_count;
  self->little_endian = little_endian;
  return object;
}

UtObject *ut_bit_list_new_msb() { return bit_list_new(NULL, 0, false); }

UtObject *ut_bit_list_new_lsb() { return bit_list_new(NULL, 0, true); }

UtObject *ut_bit_list_new_msb_from_data(UtObject *data, size_t bit_count) {
  return bit_list_new(data, bit_count, false);
}

UtObject *ut_bit_list_new_lsb_from_data(UtObject *data, size_t bit_count) {
  return bit_list_new(data, bit_count, true);
}

UtObject *ut_bit_list_new_msb_from_bin_string(const char *bin) {
  UtObject *object = ut_bit_list_new_msb();
  UtBitList *self = (UtBitList *)object;
  if (!append_bin_string(self, bin)) {
    return ut_error_new("Invalid binary string");
  }
  return object;
}

UtObject *ut_bit_list_new_lsb_from_bin_string(const char *bin) {
  UtObject *object = ut_bit_list_new_lsb();
  UtBitList *self = (UtBitList *)object;
  if (!append_bin_string(self, bin)) {
    return ut_error_new("Invalid binary string");
  }
  return object;
}

size_t ut_bit_list_get_length(UtObject *object) {
  assert(ut_object_is_bit_list(object));
  UtBitList *self = (UtBitList *)object;
  return self->bit_count;
}

bool ut_bit_list_get_element(UtObject *object, size_t index) {
  assert(ut_object_is_bit_list(object));
  UtBitList *self = (UtBitList *)object;
  return get_element(self, index);
}

UtObject *ut_bit_list_get_data(UtObject *object) {
  assert(ut_object_is_bit_list(object));
  UtBitList *self = (UtBitList *)object;
  return self->data;
}

void ut_bit_list_append(UtObject *object, bool value) {
  ut_bit_list_append_bits(object, value ? 0x01 : 0x00, 1);
}

void ut_bit_list_append_bits(UtObject *object, uint8_t data, size_t bit_count) {
  assert(ut_object_is_bit_list(object));
  UtBitList *self = (UtBitList *)object;
  append(self, data, bit_count);
}

char *ut_bit_list_to_bin_string(UtObject *object) {
  assert(ut_object_is_bit_list(object));
  UtBitList *self = (UtBitList *)object;

  UtObjectRef bin_string = ut_string_new("");
  // FIXME: Do in 8 bit blocks
  for (size_t i = 0; i < self->bit_count; i++) {
    ut_string_append(bin_string, get_element(self, i) ? "1" : "0");
  }

  return ut_string_take_text(bin_string);
}

bool ut_object_is_bit_list(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
