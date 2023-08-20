#include <assert.h>
#include <math.h>
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

  // First error that occurred during encoding.
  UtObject *error;
} UtAsn1BerEncoder;

static void set_error(UtAsn1BerEncoder *self, const char *description) {
  if (self->error == NULL) {
    self->error = ut_asn1_error_new(description);
  }
}

static void set_type_error(UtAsn1BerEncoder *self, const char *type_name,
                           UtObject *value) {
  ut_cstring_ref description =
      ut_cstring_new_printf("Unknown type %s provided for %s",
                            ut_object_get_type_name(value), type_name);
  set_error(self, description);
}

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

static size_t encode_bit_string(UtAsn1BerEncoder *self, UtObject *value) {
  // FIXME: Check is MSB
  size_t value_length = ut_bit_list_get_length(value);
  UtObject *value_data = ut_bit_list_get_data(value);
  size_t unused_bits = (ut_list_get_length(value_data) * 8) - value_length;
  return encode_uint8_list(self, value_data) + encode_byte(self, unused_bits);
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

static size_t encode_special_real(UtAsn1BerEncoder *self, uint8_t value) {
  return encode_byte(self, 0x40 | value);
}

static size_t encode_real_exponent(UtAsn1BerEncoder *self, int32_t exponent) {
  if (exponent > 0) {
    if (exponent > 0x7fffff) {
      set_error(self, "REAL exponent too large");
      return 0;
    } else if (exponent > 0x7fff) {
      return encode_byte(self, exponent & 0xff) +
             encode_byte(self, (exponent >> 8) & 0xff) +
             encode_byte(self, (exponent >> 16) & 0xff);
    } else if (exponent > 0x7f) {
      return encode_byte(self, exponent & 0xff) +
             encode_byte(self, exponent >> 8);
    } else {
      return encode_byte(self, exponent);
    }
  } else {
    uint32_t raw_exponent = exponent;
    if (exponent >= -128) {
      return encode_byte(self, raw_exponent & 0xff);
    } else if (exponent >= -32768) {
      return encode_byte(self, raw_exponent & 0xff) +
             encode_byte(self, (raw_exponent >> 8) & 0xff);
    } else if (exponent >= -8388608) {
      return encode_byte(self, raw_exponent & 0xff) +
             encode_byte(self, (raw_exponent >> 8) & 0xff) +
             encode_byte(self, (raw_exponent >> 16) & 0xff);
    } else {
      set_error(self, "REAL exponent too large");
      return 0;
    }
  }
}

static size_t encode_binary_real(UtAsn1BerEncoder *self, double value) {
  uint8_t v = 0x80;

  if (value < 0.0) {
    v |= 0x40;
    value = -value;
  }

  int exponent;
  double fraction = frexp(value, &exponent);
  while (fraction != floor(fraction)) {
    fraction *= 2;
    exponent--;
  }
  uint64_t n = (uint64_t)fraction;
  size_t length = 0;
  while (n != 0) {
    length += encode_byte(self, n & 0xff);
    n >>= 8;
  }

  size_t exponent_length = encode_real_exponent(self, exponent);
  length += exponent_length;
  if (exponent_length == 0) {
    return length;
  } else if (exponent_length >= 4) {
    v |= 3;
    length += encode_byte(self, exponent_length);
  } else {
    v |= exponent_length - 1;
  }

  return length + encode_byte(self, v);
}

static size_t encode_real(UtAsn1BerEncoder *self, double value) {
  switch (fpclassify(value)) {
  case FP_NAN:
    return encode_special_real(self, 2);
  case FP_INFINITE:
    if (signbit(value)) {
      return encode_special_real(self, 1);
    } else {
      return encode_special_real(self, 0);
    }
  case FP_ZERO:
    if (signbit(value)) {
      return encode_special_real(self, 3);
    } else {
      return 0;
    }
  default:
    return encode_binary_real(self, value);
  }
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
    set_type_error(self, "BOOLEAN", value);
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
    set_type_error(self, "INTEGER", value);
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

static size_t encode_bit_string_value(UtAsn1BerEncoder *self, UtObject *value,
                                      bool encode_tag, bool *is_constructed) {
  if (!ut_object_is_bit_list(value)) {
    set_type_error(self, "BIT STRING", value);
    return 0;
  }
  size_t length = encode_bit_string(self, value);
  if (encode_tag) {
    length += encode_definite_length(self, length);
    length += encode_identifier(self, UT_ASN1_TAG_CLASS_UNIVERSAL, false,
                                UT_ASN1_TAG_UNIVERSAL_BIT_STRING);
  }
  *is_constructed = encode_tag;
  return length;
}

static size_t encode_octet_string_value(UtAsn1BerEncoder *self, UtObject *value,
                                        bool encode_tag, bool *is_constructed) {
  if (!ut_object_implements_uint8_list(value)) {
    set_type_error(self, "OCTET STRING", value);
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
    set_type_error(self, "NULL", value);
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
    set_type_error(self, "OBJECT IDENTIFIER", value);
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

static size_t encode_real_value(UtAsn1BerEncoder *self, UtObject *value,
                                bool encode_tag, bool *is_constructed) {
  if (!ut_object_is_float64(value)) {
    set_type_error(self, "REAL", value);
    return 0;
  }
  size_t length = encode_real(self, ut_float64_get_value(value));
  if (encode_tag) {
    length += encode_definite_length(self, length);
    length += encode_identifier(self, UT_ASN1_TAG_CLASS_UNIVERSAL, false,
                                UT_ASN1_TAG_UNIVERSAL_REAL);
  }
  *is_constructed = encode_tag;
  return length;
}

static size_t encode_enumerated_value(UtAsn1BerEncoder *self, UtObject *type,
                                      UtObject *value, bool encode_tag,
                                      bool *is_constructed) {
  if (!ut_object_implements_string(value)) {
    set_type_error(self, "ENUMERATED", value);
    return 0;
  }
  int64_t number =
      ut_asn1_enumerated_type_get_value(type, ut_string_get_text(value));
  if (number < 0) {
    ut_cstring_ref description = ut_cstring_new_printf(
        "Unknown enumeration item %s", ut_string_get_text(value));
    set_error(self, description);
    return 0;
  }
  size_t length = encode_enumerated(self, number);
  if (encode_tag) {
    length += encode_definite_length(self, length);
    length += encode_identifier(self, UT_ASN1_TAG_CLASS_UNIVERSAL, false,
                                UT_ASN1_TAG_UNIVERSAL_ENUMERATED);
  }
  *is_constructed = encode_tag;
  return length;
}

static size_t encode_utf8_string_value(UtAsn1BerEncoder *self, UtObject *value,
                                       bool encode_tag, bool *is_constructed) {
  if (!ut_object_implements_string(value)) {
    set_type_error(self, "UTF8String", value);
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
    set_type_error(self, "RELATIVE-OID", value);
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

static size_t encode_components(UtAsn1BerEncoder *self, const char *type_name,
                                UtObject *components, UtObject *value) {
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

    if (ut_object_is_asn1_optional_type(component_type)) {
      if (component_value == NULL) {
        continue;
      }
      component_type = ut_asn1_optional_type_get_type(component_type);
    }

    if (component_value == NULL) {
      ut_cstring_ref description = ut_cstring_new_printf(
          "Missing %s component %s", type_name, identifier_text);
      set_error(self, description);
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
    set_type_error(self, "SEQUENCE", value);
    return 0;
  }

  size_t length = encode_components(
      self, "SEQUENCE", ut_asn1_sequence_type_get_components(type), value);
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
    set_type_error(self, "SEQUENCE OF", value);
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
    set_type_error(self, "SET", value);
    return 0;
  }

  size_t length = encode_components(
      self, "SET", ut_asn1_set_type_get_components(type), value);
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
    set_type_error(self, "SET OF", value);
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

static size_t encode_choice_value(UtAsn1BerEncoder *self, UtObject *type,
                                  UtObject *value, bool encode_tag,
                                  bool *is_constructed) {
  if (!ut_object_is_asn1_choice_value(value)) {
    set_type_error(self, "CHOICE", value);
    return 0;
  }

  const char *identifier = ut_asn1_choice_value_get_identifier(value);
  UtObject *choice_type = ut_asn1_choice_type_get_component(type, identifier);
  if (choice_type == NULL) {
    ut_cstring_ref description =
        ut_cstring_new_printf("Invalid choice component %s", identifier);
    set_error(self, description);
    *is_constructed = false;
    return 0;
  }

  return encode_value(self, choice_type, ut_asn1_choice_value_get_value(value),
                      encode_tag, is_constructed);
}

static size_t encode_tagged_value(UtAsn1BerEncoder *self, UtObject *type,
                                  UtObject *value, bool encode_tag,
                                  bool *is_constructed) {
  bool is_explicit = ut_asn1_tagged_type_get_is_explicit(type);
  bool value_is_constructed;
  size_t length = encode_value(self, ut_asn1_tagged_type_get_type(type), value,
                               is_explicit, &value_is_constructed);
  if (encode_tag) {
    length += encode_definite_length(self, length);
    length += encode_identifier(self, ut_asn1_tagged_type_get_class(type),
                                value_is_constructed,
                                ut_asn1_tagged_type_get_number(type));
  }
  *is_constructed = encode_tag || value_is_constructed;
  return length;
}

static size_t encode_value(UtAsn1BerEncoder *self, UtObject *type,
                           UtObject *value, bool encode_tag,
                           bool *is_constructed) {
  if (ut_object_is_asn1_boolean_type(type)) {
    return encode_boolean_value(self, value, encode_tag, is_constructed);
  } else if (ut_object_is_asn1_integer_type(type)) {
    return encode_integer_value(self, value, encode_tag, is_constructed);
  } else if (ut_object_is_asn1_bit_string_type(type)) {
    return encode_bit_string_value(self, value, encode_tag, is_constructed);
  } else if (ut_object_is_asn1_octet_string_type(type)) {
    return encode_octet_string_value(self, value, encode_tag, is_constructed);
  } else if (ut_object_is_asn1_null_type(type)) {
    return encode_null_value(self, value, encode_tag, is_constructed);
  } else if (ut_object_is_asn1_object_identifier_type(type)) {
    return encode_object_identifier_value(self, value, encode_tag,
                                          is_constructed);
  } else if (ut_object_is_asn1_real_type(type)) {
    return encode_real_value(self, value, encode_tag, is_constructed);
  } else if (ut_object_is_asn1_enumerated_type(type)) {
    return encode_enumerated_value(self, type, value, encode_tag,
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
  } else if (ut_object_is_asn1_choice_type(type)) {
    return encode_choice_value(self, type, value, encode_tag, is_constructed);
  } else if (ut_object_is_asn1_tagged_type(type)) {
    return encode_tagged_value(self, type, value, encode_tag, is_constructed);
  } else {
    ut_cstring_ref description = ut_cstring_new_printf(
        "Unknown ASN.1 type %s", ut_object_get_type_name(type));
    set_error(self, description);
    *is_constructed = true;
    return 0;
  }
}

static void ut_asn1_ber_encoder_encode_value(UtObject *object, UtObject *type,
                                             UtObject *value) {
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;
  bool is_constructed;
  encode_value(self, type, value, true, &is_constructed);
}

static UtObject *ut_asn1_ber_encoder_get_error(UtObject *object) {
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;
  return self->error;
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
  ut_object_unref(self->error);
}

static UtAsn1EncoderInterface asn1_encoder_interface = {
    .encode_value = ut_asn1_ber_encoder_encode_value,
    .get_error = ut_asn1_ber_encoder_get_error};

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1BerEncoder",
    .init = ut_asn1_ber_encoder_init,
    .cleanup = ut_asn1_ber_encoder_cleanup,
    .interfaces = {{&ut_asn1_encoder_id, &asn1_encoder_interface},
                   {NULL, NULL}}};

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

size_t ut_asn1_ber_encoder_encode_bit_string(UtObject *object,
                                             UtObject *value) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;
  return encode_bit_string(self, value);
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
