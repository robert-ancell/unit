#include <assert.h>
#include <string.h>

#include "ut-asn1-string.h"
#include "ut.h"

// Initial buffer size.
#define MINIMUM_BUFFER_LENGTH 1024

typedef struct {
  UtObject object;

  // Data being written from end to start.
  UtObject *buffer;
  uint8_t *buffer_data;
  size_t buffer_length;

  // Number of bytes of used data.
  size_t length;
} UtAsn1BerEncoder;

static void resize(UtAsn1BerEncoder *self, size_t required_length) {
  if (required_length <= self->buffer_length) {
    return;
  }

  size_t new_buffer_length = self->buffer_length;
  while (new_buffer_length < required_length) {
    new_buffer_length *= 2;
  }

  UtObjectRef new_buffer = ut_uint8_array_new_sized(new_buffer_length);
  uint8_t *new_buffer_data = ut_uint8_list_get_writable_data(new_buffer);
  size_t extended_length = new_buffer_length - self->buffer_length;
  for (size_t i = 0; i < self->buffer_length; i++) {
    new_buffer_data[extended_length + i] = self->buffer_data[i];
  }
  ut_object_unref(self->buffer);

  self->buffer = ut_object_ref(new_buffer);
  self->buffer_data = new_buffer_data;
  self->buffer_length = new_buffer_length;
}

static size_t encode_byte(UtAsn1BerEncoder *self, uint8_t value) {
  resize(self, self->length + 1);

  self->buffer_data[self->buffer_length - self->length - 1] = value;
  self->length++;

  return 1;
}

static size_t encode_uint8_list(UtAsn1BerEncoder *self, UtObject *value) {
  size_t value_length = ut_list_get_length(value);
  resize(self, self->length + value_length);

  for (size_t i = 0; i < value_length; i++) {
    encode_byte(self, ut_uint8_list_get_element(value, value_length - i - 1));
  }

  return value_length;
}

static size_t encode_compressed_integer(UtAsn1BerEncoder *self,
                                        uint32_t value) {
  if (value <= 0x7f) {
    encode_byte(self, value);
    return 1;
  } else if (value <= 0x3fff) {
    encode_byte(self, value & 0x7f);
    encode_byte(self, 0x80 | (value >> 7));
    return 2;
  } else if (value <= 0x1fffff) {
    encode_byte(self, value & 0x7f);
    encode_byte(self, 0x80 | ((value >> 7) & 0x7f));
    encode_byte(self, 0x80 | (value >> 14));
    return 3;
  } else if (value <= 0xfffffff) {
    encode_byte(self, value & 0x7f);
    encode_byte(self, 0x80 | ((value >> 7) & 0x7f));
    encode_byte(self, 0x80 | ((value >> 14) & 0x7f));
    encode_byte(self, 0x80 | (value >> 21));
    return 4;
  } else {
    encode_byte(self, value & 0x7f);
    encode_byte(self, 0x80 | ((value >> 7) & 0x7f));
    encode_byte(self, 0x80 | ((value >> 14) & 0x7f));
    encode_byte(self, 0x80 | ((value >> 21) & 0x7f));
    encode_byte(self, 0x80 | (value >> 28));
    return 5;
  }
}

static size_t encode_identifier(UtAsn1BerEncoder *self, UtAsn1TagClass class,
                                bool constructed, uint32_t number) {
  uint8_t octet0;
  switch (class) {
  case UT_ASN1_TAG_CLASS_UNIVERSAL:
    octet0 = 0x00;
    break;
  case UT_ASN1_TAG_CLASS_APPLICATION:
    octet0 = 0x40;
    break;
  case UT_ASN1_TAG_CLASS_CONTEXT_SPECIFIC:
    octet0 = 0x80;
    break;
  case UT_ASN1_TAG_CLASS_PRIVATE:
    octet0 = 0xc0;
    break;
  default:
    assert(false);
  }
  if (constructed) {
    octet0 |= 0x20;
  }

  if (number <= 30) {
    // Short form
    return encode_byte(self, octet0 | number);
  } else {
    // Long form
    octet0 |= 0x1f;
    return encode_compressed_integer(self, number) + encode_byte(self, octet0);
  }
}

static void ut_asn1_ber_encoder_init(UtObject *object) {
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;
  self->buffer = ut_uint8_array_new_sized(MINIMUM_BUFFER_LENGTH);
  self->buffer_data = ut_uint8_list_get_writable_data(self->buffer);
  self->buffer_length = ut_list_get_length(self->buffer);
}

static void ut_asn1_ber_encoder_cleanup(UtObject *object) {
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;
  ut_object_unref(self->buffer);
}

static UtObjectInterface object_interface = {.type_name = "UtAsn1BerEncoder",
                                             .init = ut_asn1_ber_encoder_init,
                                             .cleanup =
                                                 ut_asn1_ber_encoder_cleanup};

UtObject *ut_asn1_ber_encoder_new() {
  return ut_object_new(sizeof(UtAsn1BerEncoder), &object_interface);
}

size_t ut_asn1_ber_encoder_encode_primitive_identifier(UtObject *object,
                                                       UtAsn1TagClass class,
                                                       uint32_t number) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;

  return encode_identifier(self, class, false, number);
}

size_t ut_asn1_ber_encoder_encode_constructed_identifier(UtObject *object,
                                                         UtAsn1TagClass class,
                                                         uint32_t number) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;

  return encode_identifier(self, class, true, number);
}

size_t ut_asn1_ber_encoder_encode_definite_length(UtObject *object,
                                                  size_t length) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;

  if (length <= 0x7f) {
    encode_byte(self, length);
    return 1;
  } else if (length <= 0xffff) {
    encode_byte(self, length & 0xff);
    encode_byte(self, length >> 8);
    encode_byte(self, 0x80 | 2);
    return 3;
  } else if (length <= 0xffffff) {
    encode_byte(self, length & 0xff);
    encode_byte(self, (length >> 8) & 0xff);
    encode_byte(self, length >> 16);
    encode_byte(self, 0x80 | 3);
    return 4;
  } else if (length <= 0xffffffff) {
    encode_byte(self, length & 0xff);
    encode_byte(self, (length >> 8) & 0xff);
    encode_byte(self, (length >> 16) & 0xff);
    encode_byte(self, length >> 24);
    encode_byte(self, 0x80 | 4);
    return 5;
  } else {
    assert(false);
  }
}

size_t ut_asn1_ber_encoder_encode_boolean(UtObject *object, bool value) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;
  return encode_byte(self, value ? 0xff : 0x00);
}

size_t ut_asn1_ber_encoder_encode_integer(UtObject *object, int64_t value) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;
  if (value >= 0) {
    // Calulate number of bytes to remove leading zeros.
    size_t length = 1;
    int64_t v = value >> 8;
    while (v != 0) {
      length++;
      v >>= 8;
    }
    // Add additional zeros if leading with a one (will be interpreted as
    // negative).
    if ((value >> ((length - 1) * 8) & 0x80) != 0) {
      length++;
    }
    for (size_t i = 0; i < length; i++) {
      encode_byte(self, (value >> i * 8) & 0xff);
    }

    return length;
  } else {
    // Calulate number of bytes to remove leading ones.
    uint64_t unsigned_value = (uint64_t)value;
    size_t length = 1;
    uint64_t v = unsigned_value >> 8;
    uint64_t leading_ones = 0x00ffffffffffffff;
    while (v != leading_ones) {
      length++;
      v >>= 8;
      leading_ones >>= 8;
    }
    for (size_t i = 0; i < length; i++) {
      encode_byte(self, (unsigned_value >> i * 8) & 0xff);
    }

    return length;
  }
}

size_t ut_asn1_ber_encoder_encode_octet_string(UtObject *object,
                                               UtObject *value) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;
  return encode_uint8_list(self, value);
}

size_t ut_asn1_ber_encoder_encode_null(UtObject *object) {
  assert(ut_object_is_asn1_ber_encoder(object));
  return 0;
}

size_t ut_asn1_ber_encoder_encode_object_identifier(UtObject *object,
                                                    UtObject *identifier) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;

  size_t identifier_length = ut_list_get_length(identifier);
  assert(identifier_length >= 3);
  size_t length = 0;
  for (size_t i = 0; i < identifier_length - 2; i++) {
    length += encode_compressed_integer(
        self,
        ut_uint32_list_get_element(identifier, identifier_length - i - 1));
  }
  uint32_t subidentifier0 = ut_uint32_list_get_element(identifier, 0) * 40 +
                            ut_uint32_list_get_element(identifier, 1);
  length += encode_compressed_integer(self, subidentifier0);

  return length;
}

size_t ut_asn1_ber_encoder_encode_enumerated(UtObject *object, int64_t value) {
  return ut_asn1_ber_encoder_encode_integer(object, value);
}

size_t ut_asn1_ber_encoder_encode_utf8_string(UtObject *object,
                                              UtObject *value) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;

  UtObjectRef utf8 = ut_string_get_utf8(value);
  return encode_uint8_list(self, utf8);
}

size_t
ut_asn1_ber_encoder_encode_relative_object_identifier(UtObject *object,
                                                      UtObject *identifier) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;

  size_t identifier_length = ut_list_get_length(identifier);
  size_t length = 0;
  for (size_t i = 0; i < identifier_length; i++) {
    length += encode_compressed_integer(
        self,
        ut_uint32_list_get_element(identifier, identifier_length - i - 1));
  }
  return length;
}

size_t ut_asn1_ber_encoder_encode_numeric_string(UtObject *object,
                                                 const char *value) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;

  UtObjectRef numeric_string = ut_asn1_encode_numeric_string(value);
  return encode_uint8_list(self, numeric_string);
}

size_t ut_asn1_ber_encoder_encode_printable_string(UtObject *object,
                                                   const char *value) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;

  UtObjectRef printable_string = ut_asn1_encode_printable_string(value);
  return encode_uint8_list(self, printable_string);
}

size_t ut_asn1_ber_encoder_encode_ia5_string(UtObject *object,
                                             UtObject *value) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;

  UtObjectRef ia5_string = ut_asn1_encode_ia5_string(value);
  return encode_uint8_list(self, ia5_string);
}

size_t ut_asn1_ber_encoder_encode_visible_string(UtObject *object,
                                                 const char *value) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;

  UtObjectRef visible_string = ut_asn1_encode_visible_string(value);
  return encode_uint8_list(self, visible_string);
}

UtObject *ut_asn1_ber_encoder_get_data(UtObject *object) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;
  return ut_list_get_sublist(self->buffer, self->buffer_length - self->length,
                             self->length);
}

bool ut_object_is_asn1_ber_encoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
