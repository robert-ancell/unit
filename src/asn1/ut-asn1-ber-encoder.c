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

static size_t encode_definite_length(UtAsn1BerEncoder *self, size_t length) {
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

static size_t encode_boolean(UtAsn1BerEncoder *self, bool value) {
  return encode_byte(self, value ? 0xff : 0x00);
}

static size_t encode_integer(UtAsn1BerEncoder *self, int64_t value) {
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

static size_t encode_octet_string(UtAsn1BerEncoder *self, UtObject *value) {
  return encode_uint8_list(self, value);
}

static size_t encode_null(UtAsn1BerEncoder *self) { return 0; }

static size_t encode_object_identifier(UtAsn1BerEncoder *self,
                                       UtObject *identifier) {
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

static size_t encode_real(UtAsn1BerEncoder *self, double value) {
  assert(false);
}

static size_t encode_enumerated(UtAsn1BerEncoder *self, int64_t value) {
  return encode_integer(self, value);
}

static size_t encode_utf8_string(UtAsn1BerEncoder *self, UtObject *value) {
  UtObjectRef utf8 = ut_string_get_utf8(value);
  return encode_uint8_list(self, utf8);
}

static size_t encode_relative_oid(UtAsn1BerEncoder *self,
                                  UtObject *identifier) {
  size_t identifier_length = ut_list_get_length(identifier);
  size_t length = 0;
  for (size_t i = 0; i < identifier_length; i++) {
    length += encode_compressed_integer(
        self,
        ut_uint32_list_get_element(identifier, identifier_length - i - 1));
  }
  return length;
}

static size_t encode_numeric_string(UtAsn1BerEncoder *self, const char *value) {
  UtObjectRef numeric_string = ut_asn1_encode_numeric_string(value);
  return encode_uint8_list(self, numeric_string);
}

static size_t encode_printable_string(UtAsn1BerEncoder *self,
                                      const char *value) {
  UtObjectRef printable_string = ut_asn1_encode_printable_string(value);
  return encode_uint8_list(self, printable_string);
}

static size_t encode_ia5_string(UtAsn1BerEncoder *self, UtObject *value) {
  UtObjectRef ia5_string = ut_asn1_encode_ia5_string(value);
  return encode_uint8_list(self, ia5_string);
}

static size_t encode_visible_string(UtAsn1BerEncoder *self, const char *value) {
  UtObjectRef visible_string = ut_asn1_encode_visible_string(value);
  return encode_uint8_list(self, visible_string);
}

static size_t encode_boolean_value(UtAsn1BerEncoder *self, UtObject *value,
                                   bool encode_tag, bool *is_constructed) {
  if (!ut_object_is_boolean(value)) {
    // set_error
    assert(false);
    return 0;
  }
  size_t length = encode_boolean(self, ut_boolean_get_value(value));
  if (encode_tag) {
    length += encode_definite_length(self, length);
    length += encode_identifier(self, UT_ASN1_TAG_CLASS_UNIVERSAL, false,
                                UT_ASN1_TAG_UNIVERSAL_BOOLEAN);
  }
  *is_constructed = encode_tag;
  return length;
}

static size_t encode_integer_value(UtAsn1BerEncoder *self, UtObject *value,
                                   bool encode_tag, bool *is_constructed) {
  if (!ut_object_is_int64(value)) {
    // set_error
    assert(false);
    return 0;
  }
  size_t length = encode_integer(self, ut_int64_get_value(value));
  if (encode_tag) {
    length += encode_definite_length(self, length);
    length += encode_identifier(self, UT_ASN1_TAG_CLASS_UNIVERSAL, false,
                                UT_ASN1_TAG_UNIVERSAL_INTEGER);
  }
  *is_constructed = encode_tag;
  return length;
}

static size_t encode_octet_string_value(UtAsn1BerEncoder *self, UtObject *value,
                                        bool encode_tag, bool *is_constructed) {
  if (!ut_object_implements_uint8_list(value)) {
    // set_error
    assert(false);
    return 0;
  }
  size_t length = encode_octet_string(self, value);
  if (encode_tag) {
    length += encode_definite_length(self, length);
    length += encode_identifier(self, UT_ASN1_TAG_CLASS_UNIVERSAL, false,
                                UT_ASN1_TAG_UNIVERSAL_OCTET_STRING);
  }
  *is_constructed = encode_tag;
  return length;
}

static size_t encode_null_value(UtAsn1BerEncoder *self, UtObject *value,
                                bool encode_tag, bool *is_constructed) {
  if (!ut_object_is_null(value)) {
    // set_error
    assert(false);
    return 0;
  }
  size_t length = encode_null(self);
  if (encode_tag) {
    length += encode_definite_length(self, length);
    length += encode_identifier(self, UT_ASN1_TAG_CLASS_UNIVERSAL, false,
                                UT_ASN1_TAG_UNIVERSAL_NULL);
  }
  *is_constructed = encode_tag;
  return length;
}

static size_t encode_object_identifier_value(UtAsn1BerEncoder *self,
                                             UtObject *value, bool encode_tag,
                                             bool *is_constructed) {
  if (!ut_object_implements_uint32_list(value)) {
    // set_error
    assert(false);
    return 0;
  }
  size_t length = encode_object_identifier(self, value);
  if (encode_tag) {
    length += encode_definite_length(self, length);
    length += encode_identifier(self, UT_ASN1_TAG_CLASS_UNIVERSAL, false,
                                UT_ASN1_TAG_UNIVERSAL_OBJECT_IDENTIFIER);
  }
  *is_constructed = encode_tag;
  return length;
}

static size_t encode_utf8_string_value(UtAsn1BerEncoder *self, UtObject *value,
                                       bool encode_tag, bool *is_constructed) {
  if (!ut_object_implements_string(value)) {
    // set_error
    assert(false);
    return 0;
  }
  size_t length = encode_utf8_string(self, value);
  if (encode_tag) {
    length += encode_definite_length(self, length);
    length += encode_identifier(self, UT_ASN1_TAG_CLASS_UNIVERSAL, false,
                                UT_ASN1_TAG_UNIVERSAL_UTF8_STRING);
  }
  *is_constructed = encode_tag;
  return length;
}

static size_t encode_relative_oid_value(UtAsn1BerEncoder *self, UtObject *value,
                                        bool encode_tag, bool *is_constructed) {
  if (!ut_object_implements_uint32_list(value)) {
    // set_error
    assert(false);
    return 0;
  }
  size_t length = encode_relative_oid(self, value);
  if (encode_tag) {
    length += encode_definite_length(self, length);
    length += encode_identifier(self, UT_ASN1_TAG_CLASS_UNIVERSAL, false,
                                UT_ASN1_TAG_UNIVERSAL_RELATIVE_OID);
  }
  *is_constructed = encode_tag;
  return length;
}

static size_t encode_value(UtAsn1BerEncoder *self, UtObject *type,
                           UtObject *value, bool encode_tag,
                           bool *is_constructed);

static size_t encode_components(UtAsn1BerEncoder *self, UtObject *components,
                                UtObject *value) {
  UtObjectRef identifiers = ut_map_get_keys(components);
  size_t identifiers_length = ut_list_get_length(identifiers);
  size_t length = 0;
  for (size_t i = 0; i < identifiers_length; i++) {
    UtObjectRef identifier =
        ut_list_get_element(identifiers, identifiers_length - i - 1);
    const char *identifier_text = ut_string_get_text(identifier);
    UtObject *component_type =
        ut_map_lookup_string(components, identifier_text);
    UtObject *component_value = ut_map_lookup_string(value, identifier_text);

    if (component_value == NULL) {
      // set_error
      assert(false);
      return 0;
    }

    bool is_constructed;
    length += encode_value(self, component_type, component_value, true,
                           &is_constructed);
  }

  return length;
}

static size_t encode_sequence_value(UtAsn1BerEncoder *self, UtObject *type,
                                    UtObject *value, bool encode_tag,
                                    bool *is_constructed) {
  if (!ut_object_implements_map(value)) {
    // set_error
    assert(false);
    return 0;
  }

  size_t length = encode_components(
      self, ut_asn1_sequence_type_get_components(type), value);
  if (encode_tag) {
    length += encode_definite_length(self, length);
    length += encode_identifier(self, UT_ASN1_TAG_CLASS_UNIVERSAL, true,
                                UT_ASN1_TAG_UNIVERSAL_SEQUENCE);
  }
  *is_constructed = true;
  return length;
}

static size_t encode_value_list(UtAsn1BerEncoder *self, UtObject *type,
                                UtObject *value) {
  size_t value_length = ut_list_get_length(value);
  size_t length = 0;
  for (size_t i = 0; i < value_length; i++) {
    UtObject *child_value =
        ut_object_list_get_element(value, value_length - i - 1);
    bool is_constructed;
    length += encode_value(self, type, child_value, true, &is_constructed);
  }
  return length;
}

static size_t encode_sequence_of_value(UtAsn1BerEncoder *self, UtObject *type,
                                       UtObject *value, bool encode_tag,
                                       bool *is_constructed) {
  if (!ut_object_implements_list(value)) {
    // set_error
    assert(false);
    return 0;
  }

  size_t length =
      encode_value_list(self, ut_asn1_sequence_of_type_get_type(type), value);
  if (encode_tag) {
    length += encode_definite_length(self, length);
    length += encode_identifier(self, UT_ASN1_TAG_CLASS_UNIVERSAL, true,
                                UT_ASN1_TAG_UNIVERSAL_SEQUENCE);
  }
  *is_constructed = true;
  return length;
}

static size_t encode_set_value(UtAsn1BerEncoder *self, UtObject *type,
                               UtObject *value, bool encode_tag,
                               bool *is_constructed) {
  if (!ut_object_implements_map(value)) {
    // set_error
    assert(false);
    return 0;
  }

  size_t length =
      encode_components(self, ut_asn1_set_type_get_components(type), value);
  if (encode_tag) {
    length += encode_definite_length(self, length);
    length += encode_identifier(self, UT_ASN1_TAG_CLASS_UNIVERSAL, true,
                                UT_ASN1_TAG_UNIVERSAL_SET);
  }
  *is_constructed = true;
  return length;
}

static size_t encode_set_of_value(UtAsn1BerEncoder *self, UtObject *type,
                                  UtObject *value, bool encode_tag,
                                  bool *is_constructed) {
  if (!ut_object_implements_list(value)) {
    // set_error
    assert(false);
    return 0;
  }

  size_t length =
      encode_value_list(self, ut_asn1_set_of_type_get_type(type), value);
  if (encode_tag) {
    length += encode_definite_length(self, length);
    length += encode_identifier(self, UT_ASN1_TAG_CLASS_UNIVERSAL, true,
                                UT_ASN1_TAG_UNIVERSAL_SET);
  }
  *is_constructed = true;
  return length;
}

static size_t encode_value(UtAsn1BerEncoder *self, UtObject *type,
                           UtObject *value, bool encode_tag,
                           bool *is_constructed) {
  if (ut_object_is_asn1_boolean_type(type)) {
    return encode_boolean_value(self, value, encode_tag, is_constructed);
  } else if (ut_object_is_asn1_integer_type(type)) {
    return encode_integer_value(self, value, encode_tag, is_constructed);
  } else if (ut_object_is_asn1_octet_string_type(type)) {
    return encode_octet_string_value(self, value, encode_tag, is_constructed);
  } else if (ut_object_is_asn1_null_type(type)) {
    return encode_null_value(self, value, encode_tag, is_constructed);
  } else if (ut_object_is_asn1_object_identifier_type(type)) {
    return encode_object_identifier_value(self, value, encode_tag,
                                          is_constructed);
  } else if (ut_object_is_asn1_utf8_string_type(type)) {
    return encode_utf8_string_value(self, value, encode_tag, is_constructed);
  } else if (ut_object_is_asn1_relative_oid_type(type)) {
    return encode_relative_oid_value(self, value, encode_tag, is_constructed);
  } else if (ut_object_is_asn1_sequence_type(type)) {
    return encode_sequence_value(self, type, value, encode_tag, is_constructed);
  } else if (ut_object_is_asn1_sequence_of_type(type)) {
    return encode_sequence_of_value(self, type, value, encode_tag,
                                    is_constructed);
  } else if (ut_object_is_asn1_set_type(type)) {
    return encode_set_value(self, type, value, encode_tag, is_constructed);
  } else if (ut_object_is_asn1_set_of_type(type)) {
    return encode_set_of_value(self, type, value, encode_tag, is_constructed);
  } else {
    // set_error
    assert(false);
    *is_constructed = true;
    return 0;
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

size_t ut_asn1_ber_encoder_encode_value(UtObject *object, UtObject *type,
                                        UtObject *value) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;
  bool is_constructed;
  return encode_value(self, type, value, true, &is_constructed);
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
  return encode_definite_length(self, length);
}

size_t ut_asn1_ber_encoder_encode_boolean(UtObject *object, bool value) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;
  return encode_boolean(self, value);
}

size_t ut_asn1_ber_encoder_encode_integer(UtObject *object, int64_t value) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;
  return encode_integer(self, value);
}

size_t ut_asn1_ber_encoder_encode_octet_string(UtObject *object,
                                               UtObject *value) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;
  return encode_octet_string(self, value);
}

size_t ut_asn1_ber_encoder_encode_null(UtObject *object) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;
  return encode_null(self);
}

size_t ut_asn1_ber_encoder_encode_object_identifier(UtObject *object,
                                                    UtObject *identifier) {
  assert(ut_object_is_asn1_ber_encoder(object));
  assert(ut_object_implements_uint32_list(identifier));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;
  return encode_object_identifier(self, identifier);
}

size_t ut_asn1_ber_encoder_encode_real(UtObject *object, double value) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;
  return encode_real(self, value);
}

size_t ut_asn1_ber_encoder_encode_enumerated(UtObject *object, int64_t value) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;
  return encode_enumerated(self, value);
}

size_t ut_asn1_ber_encoder_encode_utf8_string(UtObject *object,
                                              UtObject *value) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;
  return encode_utf8_string(self, value);
}

size_t ut_asn1_ber_encoder_encode_relative_oid(UtObject *object,
                                               UtObject *identifier) {
  assert(ut_object_is_asn1_ber_encoder(object));
  assert(ut_object_implements_uint32_list(identifier));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;
  return encode_relative_oid(self, identifier);
}

size_t ut_asn1_ber_encoder_encode_numeric_string(UtObject *object,
                                                 const char *value) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;
  return encode_numeric_string(self, value);
}

size_t ut_asn1_ber_encoder_encode_printable_string(UtObject *object,
                                                   const char *value) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;
  return encode_printable_string(self, value);
}

size_t ut_asn1_ber_encoder_encode_ia5_string(UtObject *object,
                                             UtObject *value) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;
  return encode_ia5_string(self, value);
}

size_t ut_asn1_ber_encoder_encode_visible_string(UtObject *object,
                                                 const char *value) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;
  return encode_visible_string(self, value);
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
