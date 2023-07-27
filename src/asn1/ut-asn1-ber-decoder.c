#include <assert.h>
#include <math.h>

#include "ut-asn1-string.h"
#include "ut.h"

typedef struct {
  UtObject object;

  // Tag on this contents.
  UtObject *tag;

  // True if contents are more tagged values.
  bool constructed;

  // Contents of this value.
  UtObject *contents;

  // Total number of bytes this value uses.
  size_t length;

  // First error that occurred during decoding.
  UtObject *error;
} UtAsn1BerDecoder;

static void set_error(UtAsn1BerDecoder *self, const char *description) {
  if (self->error == NULL) {
    self->error = ut_asn1_error_new(description);
  }
}

static bool decode_compressed_integer(UtObject *data, size_t *offset,
                                      uint32_t *value) {
  size_t data_length = ut_list_get_length(data);

  // FIXME: Only support 5 bytes (32 bits)
  uint32_t value_ = 0;
  while (*offset < data_length) {
    uint8_t number_octet = ut_uint8_list_get_element(data, *offset);
    (*offset)++;
    value_ = value_ << 7 | (number_octet & 0x7f);
    if ((number_octet & 0x80) == 0) {
      *value = value_;
      return value;
    }
  }

  return false;
}

static UtObject *decode_constructed(UtAsn1BerDecoder *self) {
  UtObjectRef children = ut_list_new();

  size_t offset = 0;
  size_t contents_length = ut_list_get_length(self->contents);
  while (offset < contents_length) {
    UtObjectRef data =
        ut_list_get_sublist(self->contents, offset, contents_length - offset);
    UtObjectRef child = ut_asn1_ber_decoder_new(data);
    UtObject *child_error = ut_asn1_decoder_get_error(child);
    if (child_error != NULL) {
      ut_cstring_ref child_description = ut_error_get_description(child_error);
      ut_cstring_ref description = ut_cstring_new_printf(
          "Failed to decode child: %s", child_description);
      set_error(self, description);
      return ut_list_new();
    }
    ut_list_append(children, child);
    offset += ut_asn1_ber_decoder_get_length(child);
  }

  return ut_object_ref(children);
}

static bool decode_boolean(UtAsn1BerDecoder *self) {
  size_t data_length = ut_list_get_length(self->contents);
  if (data_length != 1) {
    set_error(self, "Invalid BOOLEAN data length");
    return false;
  }
  if (self->constructed) {
    set_error(self, "BOOLEAN does not have constructed form");
    return false;
  }
  // FIXME: If in DER mode then generate error if invalid.
  return ut_uint8_list_get_element(self->contents, 0) != 0;
}

static int64_t decode_integer(UtAsn1BerDecoder *self, const char *type_name) {
  size_t data_length = ut_list_get_length(self->contents);
  if (data_length == 0) {
    ut_cstring_ref description =
        ut_cstring_new_printf("Invalid %s data length", type_name);
    set_error(self, description);
    return 0;
  }
  if (data_length > 8) {
    ut_cstring_ref description = ut_cstring_new_printf(
        "%s greater than 64 bits not supported", type_name);
    set_error(self, description);
    return 0;
  }
  if (self->constructed) {
    ut_cstring_ref description =
        ut_cstring_new_printf("%s does not have constructed form", type_name);
    set_error(self, description);
    return 0;
  }
  size_t offset = 0;
  uint8_t v0 = ut_uint8_list_get_element(self->contents, offset);
  offset++;
  uint64_t value;
  if ((v0 & 0x80) != 0) {
    // Leading 1 is a negative number - extend to 64 bits.
    value = 0xffffffffffffff00 | v0;
  } else {
    value = v0;
  }
  while (offset < data_length) {
    value = value << 8 | ut_uint8_list_get_element(self->contents, offset);
    offset++;
  }

  return (int64_t)value;
}

static UtObject *decode_primitive_bit_string(UtAsn1BerDecoder *self) {
  size_t data_length = ut_list_get_length(self->contents);
  if (data_length < 1) {
    set_error(self, "Invalid BIT STRING data length");
    return ut_bit_list_new_msb();
  }
  uint8_t unused_bits = ut_uint8_list_get_element(self->contents, 0);
  if (unused_bits > 7) {
    set_error(self, "Invalid number of unused bits in BIT STRING");
    return ut_bit_list_new_msb();
  }
  size_t bit_count = (data_length - 1) * 8 - unused_bits;
  UtObjectRef bit_data =
      ut_list_get_sublist(self->contents, 1, data_length - 1);
  return ut_bit_list_new_msb_from_data(bit_data, bit_count);
}

static UtObject *decode_constructed_bit_string(UtAsn1BerDecoder *self) {
  UtObjectRef value = ut_bit_list_new_msb();
  size_t contents_length = ut_list_get_length(self->contents);
  size_t offset = 0;
  while (offset < contents_length) {
    UtObjectRef data =
        ut_list_get_sublist(self->contents, offset, contents_length - offset);
    UtObjectRef decoder = ut_asn1_ber_decoder_new(data);
    offset += ut_asn1_ber_decoder_get_length(decoder);

    if (!ut_object_equal(self->tag, ut_asn1_ber_decoder_get_tag(decoder))) {
      set_error(self, "Invalid tag inside constructed BIT STRING");
      return ut_bit_list_new_msb();
    }
    if (ut_asn1_ber_decoder_get_constructed(decoder)) {
      set_error(self, "Invalid nested constructed BIT STRING");
      return ut_bit_list_new_msb();
    }

    UtObjectRef child_value =
        decode_primitive_bit_string((UtAsn1BerDecoder *)decoder);
    UtObject *error = ut_asn1_decoder_get_error(decoder);
    if (error != NULL) {
      ut_cstring_ref child_description = ut_error_get_description(error);
      ut_cstring_ref description = ut_cstring_new_printf(
          "Error decoding constructed BIT STRING element: %s",
          child_description);
      set_error(self, description);
      return ut_bit_list_new_msb();
    }
    ut_bit_list_append_list(value, child_value);
  }

  return ut_object_ref(value);
}

static UtObject *decode_bit_string(UtAsn1BerDecoder *self) {
  if (self->constructed) {
    return decode_constructed_bit_string(self);
  }

  return decode_primitive_bit_string(self);
}

static UtObject *decode_constructed_octet_string(UtAsn1BerDecoder *self,
                                                 const char *type_name) {
  UtObjectRef value = ut_uint8_list_new();
  size_t contents_length = ut_list_get_length(self->contents);
  size_t offset = 0;
  while (offset < contents_length) {
    UtObjectRef data =
        ut_list_get_sublist(self->contents, offset, contents_length - offset);
    UtObjectRef decoder = ut_asn1_ber_decoder_new(data);
    offset += ut_asn1_ber_decoder_get_length(decoder);

    if (!ut_object_equal(self->tag, ut_asn1_ber_decoder_get_tag(decoder))) {
      ut_cstring_ref description =
          ut_cstring_new_printf("Invalid tag inside constructed %s", type_name);
      set_error(self, description);
      return ut_uint8_list_new();
    }
    if (ut_asn1_ber_decoder_get_constructed(decoder)) {
      ut_cstring_ref description =
          ut_cstring_new_printf("Invalid nested constructed %s", type_name);
      set_error(self, description);
      return ut_uint8_list_new();
    }

    ut_list_append_list(value, ut_asn1_ber_decoder_get_contents(decoder));
    UtObject *error = ut_asn1_decoder_get_error(decoder);
    if (error != NULL) {
      ut_cstring_ref child_description = ut_error_get_description(error);
      ut_cstring_ref description =
          ut_cstring_new_printf("Error decoding constructed %s element: %s",
                                type_name, child_description);
      set_error(self, description);
      return ut_uint8_list_new();
    }
  }

  return ut_object_ref(value);
}

static UtObject *decode_octet_string(UtAsn1BerDecoder *self,
                                     const char *type_name) {
  if (self->constructed) {
    return decode_constructed_octet_string(self, type_name);
  }
  return ut_object_ref(self->contents);
}

static void decode_null(UtAsn1BerDecoder *self) {
  size_t data_length = ut_list_get_length(self->contents);
  if (data_length != 0) {
    set_error(self, "Invalid NULL data length");
    return;
  }
  if (self->constructed) {
    set_error(self, "NULL does not have constructed form");
    return;
  }
}

static UtObject *decode_object_identifier(UtAsn1BerDecoder *self) {
  size_t data_length = ut_list_get_length(self->contents);
  UtObjectRef identifier = ut_uint32_list_new();

  if (self->constructed) {
    set_error(self, "OBJECT IDENTIFIER does not have constructed form");
    return ut_object_ref(identifier);
  }

  size_t offset = 0;
  uint32_t subidentifier0;
  if (!decode_compressed_integer(self->contents, &offset, &subidentifier0)) {
    set_error(self, "Invalid OBJECT IDENTIFIER");
    return ut_object_ref(identifier);
  }
  if (subidentifier0 <= 39) {
    ut_uint32_list_append(identifier, 0);
    ut_uint32_list_append(identifier, subidentifier0);
  } else if (subidentifier0 <= 79) {
    ut_uint32_list_append(identifier, 1);
    ut_uint32_list_append(identifier, subidentifier0 - 40);
  } else {
    ut_uint32_list_append(identifier, 2);
    ut_uint32_list_append(identifier, subidentifier0 - 80);
  }
  while (offset < data_length) {
    uint32_t subidentifier;
    if (!decode_compressed_integer(self->contents, &offset, &subidentifier)) {
      set_error(self, "Invalid OBJECT IDENTIFIER");
      return ut_object_ref(identifier);
    }
    ut_uint32_list_append(identifier, subidentifier);
  }
  return ut_object_ref(identifier);
}

static double decode_binary_real(UtAsn1BerDecoder *self) {
  size_t data_length = ut_list_get_length(self->contents);

  size_t offset = 0;
  uint8_t v = ut_uint8_list_get_element(self->contents, offset);
  offset++;

  double sign = (v & 0x40) != 0 ? -1.0 : 1.0;

  uint8_t base;
  switch ((v >> 4) & 0x3) {
  case 0:
    base = 2;
    break;
  case 1:
    base = 8;
    break;
  case 2:
    base = 16;
    break;
  default:
    set_error(self, "Invalid REAL base");
    return 0.0;
  }

  uint32_t scaling_factor = 1 << ((v >> 2) & 0x3);

  size_t exponent_length;
  switch (v & 0x3) {
  case 0:
    exponent_length = 1;
    break;
  case 1:
    exponent_length = 2;
    break;
  case 2:
    exponent_length = 3;
    break;
  case 3:
    if (data_length < 2) {
      set_error(self, "Insufficient space for REAL exponent");
      return 0.0;
    }
    exponent_length = ut_uint8_list_get_element(self->contents, offset);
    if (exponent_length == 0) {
      set_error(self, "Invalid REAL exponent length");
      return 0.0;
    }
    offset++;
    break;
  }
  if (exponent_length > 4) {
    set_error(self, "Unsupported REAL exponent length");
    return 0.0;
  }
  if (offset + exponent_length > data_length) {
    set_error(self, "Insufficient space for REAL exponent");
    return 0.0;
  }

  uint32_t raw_exponent = ut_uint8_list_get_element(self->contents, offset);
  offset++;
  // If negative, extend leading ones.
  if ((raw_exponent & 0x80) != 0) {
    raw_exponent |= 0xffffff00;
  }
  for (size_t i = 1; i < exponent_length; i++) {
    raw_exponent =
        raw_exponent << 8 | ut_uint8_list_get_element(self->contents, offset);
    offset++;
  }
  int32_t exponent = raw_exponent;

  uint64_t n = 0;
  while (offset < data_length) {
    n = n << 8 | ut_uint8_list_get_element(self->contents, offset);
    offset++;
  }

  if (exponent >= 0) {
    return sign * n * scaling_factor * pow(base, exponent);
  } else {
    return sign * n * scaling_factor / pow(base, -exponent);
  }
}

static double decode_special_real(UtAsn1BerDecoder *self) {
  size_t data_length = ut_list_get_length(self->contents);
  if (data_length != 1) {
    set_error(self, "Invalid length REAL special value");
    return 0.0;
  }

  uint8_t v = ut_uint8_list_get_element(self->contents, 0);
  switch (v & 0x3f) {
  case 0:
    return INFINITY;
  case 1:
    return -INFINITY;
  case 2:
    return NAN;
  case 3:
    return -0.0;
  default:
    set_error(self, "Invalid REAL special value");
    return 0.0;
  }
}

static double decode_decimal_real(UtAsn1BerDecoder *self, uint8_t form) {
  switch (form) {
  case 1:
    set_error(self, "REAL ISO 6093 NR1 decimal encoding not supported");
    return 0.0;
  case 2:
    set_error(self, "REAL ISO 6093 NR2 decimal encoding not supported");
    return 0.0;
  case 3:
    set_error(self, "REAL ISO 6093 NR3 decimal encoding not supported");
    return 0.0;
  default:
    set_error(self, "Invalid REAL decimal encoding");
    return 0.0;
  }
}

static double decode_real(UtAsn1BerDecoder *self) {
  if (self->constructed) {
    set_error(self, "REAL does not have constructed form");
    return 0.0;
  }

  size_t data_length = ut_list_get_length(self->contents);
  if (data_length == 0) {
    return 0.0;
  }

  uint8_t v = ut_uint8_list_get_element(self->contents, 0);
  if ((v & 0x80) != 0) {
    return decode_binary_real(self);
  } else {
    if ((v & 0x40) != 0) {
      return decode_special_real(self);
    } else {
      return decode_decimal_real(self, v);
    }
  }
}

static int64_t decode_enumerated(UtAsn1BerDecoder *self) {
  return decode_integer(self, "ENUMERATED");
}

static UtObject *decode_utf8_string(UtAsn1BerDecoder *self) {
  UtObjectRef data = decode_octet_string(self, "UTF8String");
  UtObjectRef value = ut_string_new_from_utf8(data);
  if (ut_object_implements_error(value)) {
    set_error(self, "Invalid Utf8String");
    return ut_string_new("");
  }

  return ut_object_ref(value);
}

static UtObject *decode_numeric_string(UtAsn1BerDecoder *self) {
  UtObjectRef data = decode_octet_string(self, "NumericString");
  UtObjectRef value = ut_asn1_decode_numeric_string(data);
  if (value == NULL) {
    set_error(self, "Invalid NumericString");
    return ut_string_new("");
  }

  return ut_object_ref(value);
}

static UtObject *decode_printable_string(UtAsn1BerDecoder *self) {
  UtObjectRef data = decode_octet_string(self, "PrintableString");
  UtObjectRef value = ut_asn1_decode_printable_string(data);
  if (value == NULL) {
    set_error(self, "Invalid PrintableString");
    return ut_string_new("");
  }

  return ut_object_ref(value);
}

static UtObject *decode_ia5_string(UtAsn1BerDecoder *self) {
  UtObjectRef data = decode_octet_string(self, "IA5String");
  UtObjectRef value = ut_asn1_decode_ia5_string(data);
  if (value == NULL) {
    set_error(self, "Invalid IA5String");
    return ut_string_new("");
  }

  return ut_object_ref(value);
}

static UtObject *decode_graphic_string(UtAsn1BerDecoder *self,
                                       const char *type_name) {
  UtObjectRef data = decode_octet_string(self, type_name);
  UtObjectRef value = ut_asn1_decode_graphic_string(data);
  if (value == NULL) {
    ut_cstring_ref description = ut_cstring_new_printf("Invalid %s", type_name);
    set_error(self, description);
    return ut_string_new("");
  }

  return ut_object_ref(value);
}

static UtObject *decode_object_descriptor(UtAsn1BerDecoder *self) {
  return decode_graphic_string(self, "ObjectDescriptor");
}

static UtObject *decode_visible_string(UtAsn1BerDecoder *self) {
  UtObjectRef data = decode_octet_string(self, "VisibleString");
  UtObjectRef value = ut_asn1_decode_visible_string(data);
  if (value == NULL) {
    set_error(self, "Invalid VisibleString");
    return ut_string_new("");
  }

  return ut_object_ref(value);
}

static UtObject *decode_general_string(UtAsn1BerDecoder *self) {
  UtObjectRef data = decode_octet_string(self, "GeneralString");
  UtObjectRef value = ut_asn1_decode_general_string(data);
  if (value == NULL) {
    set_error(self, "Invalid GeneralString");
    return ut_string_new("");
  }

  return ut_object_ref(value);
}

static UtObject *decode_relative_oid(UtAsn1BerDecoder *self) {
  size_t data_length = ut_list_get_length(self->contents);
  UtObjectRef identifier = ut_uint32_list_new();

  if (self->constructed) {
    set_error(self, "RELATIVE-OID does not have constructed form");
    return ut_object_ref(identifier);
  }

  size_t offset = 0;
  while (offset < data_length) {
    uint32_t subidentifier;
    if (!decode_compressed_integer(self->contents, &offset, &subidentifier)) {
      set_error(self, "Invalid RELATIVE-OID");
      return ut_object_ref(identifier);
    }
    ut_uint32_list_append(identifier, subidentifier);
  }
  return ut_object_ref(identifier);
}

static bool expect_tag(UtAsn1BerDecoder *self, UtAsn1TagClass class,
                       uint32_t number) {
  if (!ut_asn1_tag_matches(self->tag, class, number)) {
    UtObjectRef expected_tag = ut_asn1_tag_new(class, number);
    ut_cstring_ref expected_tag_string = ut_asn1_tag_to_string(expected_tag);
    ut_cstring_ref received_tag_string = ut_asn1_tag_to_string(self->tag);
    ut_cstring_ref description = ut_cstring_new_printf(
        "Expected tag %s, got %s", expected_tag_string, received_tag_string);
    set_error(self, description);
    return false;
  }

  return true;
}

static UtObject *decode_boolean_value(UtAsn1BerDecoder *self, bool decode_tag) {
  if (decode_tag && !expect_tag(self, UT_ASN1_TAG_CLASS_UNIVERSAL,
                                UT_ASN1_TAG_UNIVERSAL_BOOLEAN)) {
    return ut_boolean_new(false);
  }
  return ut_boolean_new(decode_boolean(self));
}

static UtObject *decode_integer_value(UtAsn1BerDecoder *self, bool decode_tag) {
  if (decode_tag && !expect_tag(self, UT_ASN1_TAG_CLASS_UNIVERSAL,
                                UT_ASN1_TAG_UNIVERSAL_INTEGER)) {
    return ut_int64_new(0);
  }
  return ut_int64_new(decode_integer(self, "INTEGER"));
}

static UtObject *decode_bit_string_value(UtAsn1BerDecoder *self,
                                         bool decode_tag) {
  if (decode_tag && !expect_tag(self, UT_ASN1_TAG_CLASS_UNIVERSAL,
                                UT_ASN1_TAG_UNIVERSAL_BIT_STRING)) {
    return ut_bit_list_new_msb();
  }
  return decode_bit_string(self);
}

static UtObject *decode_octet_string_value(UtAsn1BerDecoder *self,
                                           bool decode_tag) {
  if (decode_tag && !expect_tag(self, UT_ASN1_TAG_CLASS_UNIVERSAL,
                                UT_ASN1_TAG_UNIVERSAL_OCTET_STRING)) {
    return ut_uint8_list_new();
  }
  return decode_octet_string(self, "OCTET STRING");
}

static UtObject *decode_null_value(UtAsn1BerDecoder *self, bool decode_tag) {
  if (decode_tag && !expect_tag(self, UT_ASN1_TAG_CLASS_UNIVERSAL,
                                UT_ASN1_TAG_UNIVERSAL_NULL)) {
    return ut_int64_new(0);
  }
  decode_null(self);
  return ut_null_new();
}

static UtObject *decode_object_identifier_value(UtAsn1BerDecoder *self,
                                                bool decode_tag) {
  if (decode_tag && !expect_tag(self, UT_ASN1_TAG_CLASS_UNIVERSAL,
                                UT_ASN1_TAG_UNIVERSAL_OBJECT_IDENTIFIER)) {
    return ut_uint32_list_new();
  }
  return decode_object_identifier(self);
}

static UtObject *decode_object_descriptor_value(UtAsn1BerDecoder *self,
                                                bool decode_tag) {
  if (decode_tag && !expect_tag(self, UT_ASN1_TAG_CLASS_UNIVERSAL,
                                UT_ASN1_TAG_UNIVERSAL_OBJECT_DESCRIPTOR)) {
    return ut_uint32_list_new();
  }
  return decode_object_descriptor(self);
}

static UtObject *decode_real_value(UtAsn1BerDecoder *self, bool decode_tag) {
  if (decode_tag && !expect_tag(self, UT_ASN1_TAG_CLASS_UNIVERSAL,
                                UT_ASN1_TAG_UNIVERSAL_REAL)) {
    return ut_float64_new(0.0);
  }
  return ut_float64_new(decode_real(self));
}

static UtObject *decode_enumerated_value(UtAsn1BerDecoder *self, UtObject *type,
                                         bool decode_tag) {
  if (decode_tag && !expect_tag(self, UT_ASN1_TAG_CLASS_UNIVERSAL,
                                UT_ASN1_TAG_UNIVERSAL_ENUMERATED)) {
    return ut_string_new("");
  }

  int64_t value = decode_enumerated(self);
  const char *name = ut_asn1_enumerated_type_get_name(type, value);
  if (name == NULL) {
    if (ut_asn1_enumerated_type_get_extensible(type)) {
      return ut_string_new_printf("%li", value);
    } else {
      ut_cstring_ref description =
          ut_cstring_new_printf("Unknown enumeration value %li", value);
      set_error(self, description);
      return ut_string_new("");
    }
  }
  return ut_string_new(name);
}

static UtObject *decode_utf8_string_value(UtAsn1BerDecoder *self,
                                          bool decode_tag) {
  if (decode_tag && !expect_tag(self, UT_ASN1_TAG_CLASS_UNIVERSAL,
                                UT_ASN1_TAG_UNIVERSAL_UTF8_STRING)) {
    return ut_string_new("");
  }
  return decode_utf8_string(self);
}

static UtObject *decode_relative_oid_value(UtAsn1BerDecoder *self,
                                           bool decode_tag) {
  if (decode_tag && !expect_tag(self, UT_ASN1_TAG_CLASS_UNIVERSAL,
                                UT_ASN1_TAG_UNIVERSAL_RELATIVE_OID)) {
    return ut_uint32_list_new();
  }
  return decode_relative_oid(self);
}

static UtObject *decode_sequence_value(UtAsn1BerDecoder *self, UtObject *type,
                                       bool decode_tag) {
  if (decode_tag && !expect_tag(self, UT_ASN1_TAG_CLASS_UNIVERSAL,
                                UT_ASN1_TAG_UNIVERSAL_SEQUENCE)) {
    return ut_map_new();
  }
  if (!self->constructed) {
    set_error(self, "SEQUENCE does not have primitive form");
    return ut_map_new();
  }

  bool extensible = ut_asn1_sequence_type_get_extensible(type);
  UtObject *components = ut_asn1_sequence_type_get_components(type);
  size_t components_length = ut_map_get_length(components);
  UtObjectRef component_items = ut_map_get_items(components);
  size_t next_component = 0;

  UtObjectRef value = ut_map_new();
  size_t contents_length = ut_list_get_length(self->contents);
  size_t offset = 0;
  size_t required_component_count = 0;
  while (offset < contents_length) {
    UtObjectRef data =
        ut_list_get_sublist(self->contents, offset, contents_length - offset);
    UtObjectRef decoder = ut_asn1_ber_decoder_new(data);
    offset += ut_asn1_ber_decoder_get_length(decoder);

    // Find the next component that matches this tag.
    UtObjectRef component_name = NULL;
    UtObjectRef component_type = NULL;
    while (next_component < components_length) {
      UtObjectRef item = ut_list_get_element(component_items, next_component);
      UtObjectRef name = ut_map_item_get_key(item);
      UtObjectRef type = ut_map_item_get_value(item);
      next_component++;

      // This component matches.
      if (ut_asn1_type_matches_tag(type,
                                   ut_asn1_ber_decoder_get_tag(decoder))) {
        ut_object_set(&component_name, name);
        if (ut_object_is_asn1_optional_type(type)) {
          ut_object_set(&component_type, ut_asn1_optional_type_get_type(type));
        } else if (ut_object_is_asn1_default_type(type)) {
          ut_object_set(&component_type, ut_asn1_default_type_get_type(type));
        } else {
          ut_object_set(&component_type, type);
          required_component_count++;
        }
        break;
      }

      // Optional and default components are allowed to be missing.
      if (ut_object_is_asn1_optional_type(type)) {
        continue;
      } else if (ut_object_is_asn1_default_type(type)) {
        ut_map_insert_string(value, ut_string_get_text(component_name),
                             ut_asn1_default_type_get_default_value(type));
        continue;
      }

      ut_cstring_ref description = ut_cstring_new_printf(
          "Required SEQUENCE component %s missing", ut_string_get_text(name));
      set_error(self, description);
      return ut_map_new();
    }

    if (component_name == NULL) {
      // If extensible, ignore all additional components.
      if (extensible) {
        continue;
      }
      set_error(self, "Too many SEQUENCE components");
      return ut_map_new();
    }

    UtObjectRef component_value =
        ut_asn1_decoder_decode_value(decoder, component_type);
    UtObject *error = ut_asn1_decoder_get_error(decoder);
    if (error != NULL) {
      ut_cstring_ref child_description = ut_error_get_description(error);
      ut_cstring_ref description = ut_cstring_new_printf(
          "Error decoding SEQUENCE component %s: %s",
          ut_string_get_text(component_name), child_description);
      set_error(self, description);
      return ut_map_new();
    }

    ut_map_insert_string(value, ut_string_get_text(component_name),
                         component_value);
  }

  // Set any remaining default components.
  for (size_t i = next_component; i < components_length; i++) {
    UtObject *item = ut_object_list_get_element(component_items, i);
    UtObjectRef type = ut_map_item_get_value(item);
    if (ut_object_is_asn1_default_type(type)) {
      UtObjectRef name = ut_map_item_get_key(item);
      ut_map_insert_string(value, ut_string_get_text(name),
                           ut_asn1_default_type_get_default_value(type));
      continue;
    }
  }

  UtObjectRef component_types = ut_map_get_values(components);
  size_t total_required_components = 0;
  for (size_t i = 0; i < components_length; i++) {
    UtObject *component_type = ut_object_list_get_element(component_types, i);
    if (ut_object_is_asn1_optional_type(component_type) ||
        ut_object_is_asn1_default_type(component_type)) {
      continue;
    }
    total_required_components++;
  }

  if (required_component_count < total_required_components) {
    // FIXME: List missing components.
    set_error(self, "Required SEQUENCE components missing");
    return ut_map_new();
  }

  return ut_object_ref(value);
}

static UtObject *decode_value_list(UtAsn1BerDecoder *self, UtObject *type,
                                   const char *type_string) {
  UtObjectRef value = ut_list_new();
  size_t contents_length = ut_list_get_length(self->contents);
  size_t offset = 0;
  while (offset < contents_length) {
    UtObjectRef data =
        ut_list_get_sublist(self->contents, offset, contents_length - offset);
    UtObjectRef decoder = ut_asn1_ber_decoder_new(data);
    UtObjectRef child_value = ut_asn1_decoder_decode_value(decoder, type);
    UtObject *error = ut_asn1_decoder_get_error(decoder);
    if (error != NULL) {
      ut_cstring_ref child_description = ut_error_get_description(error);
      ut_cstring_ref description = ut_cstring_new_printf(
          "Error decoding %s element %zi: %s", type_string,
          ut_list_get_length(value), child_description);
      set_error(self, description);
      return ut_list_new();
    }
    offset += ut_asn1_ber_decoder_get_length(decoder);
    ut_list_append(value, child_value);
  }

  return ut_object_ref(value);
}

static UtObject *decode_sequence_of_value(UtAsn1BerDecoder *self,
                                          UtObject *type, bool decode_tag) {
  if (decode_tag && !expect_tag(self, UT_ASN1_TAG_CLASS_UNIVERSAL,
                                UT_ASN1_TAG_UNIVERSAL_SEQUENCE)) {
    return ut_list_new();
  }
  if (!self->constructed) {
    set_error(self, "SEQUENCE OF does not have primitive form");
    return ut_list_new();
  }

  return decode_value_list(self, ut_asn1_sequence_of_type_get_type(type),
                           "SEQUENCE OF");
}

static UtObject *match_component(UtObject *decoder, UtObject *components) {
  UtObjectRef items = ut_map_get_items(components);
  size_t items_length = ut_list_get_length(items);
  for (size_t i = 0; i < items_length; i++) {
    UtObject *item = ut_object_list_get_element(items, i);
    UtObjectRef item_type = ut_map_item_get_value(item);
    if (ut_asn1_type_matches_tag(item_type,
                                 ut_asn1_ber_decoder_get_tag(decoder))) {
      return ut_map_item_get_key(item);
    }
  }

  return NULL;
}

static UtObject *decode_set_value(UtAsn1BerDecoder *self, UtObject *type,
                                  bool decode_tag) {
  if (decode_tag && !expect_tag(self, UT_ASN1_TAG_CLASS_UNIVERSAL,
                                UT_ASN1_TAG_UNIVERSAL_SET)) {
    return ut_map_new();
  }
  if (!self->constructed) {
    set_error(self, "SET does not have primitive form");
    return ut_map_new();
  }

  bool extensible = ut_asn1_set_type_get_extensible(type);
  UtObject *components = ut_asn1_set_type_get_components(type);
  size_t components_length = ut_map_get_length(components);

  UtObjectRef value = ut_map_new();
  size_t contents_length = ut_list_get_length(self->contents);
  size_t offset = 0;
  size_t required_component_count = 0;
  while (offset < contents_length) {
    UtObjectRef data =
        ut_list_get_sublist(self->contents, offset, contents_length - offset);
    UtObjectRef decoder = ut_asn1_ber_decoder_new(data);
    offset += ut_asn1_ber_decoder_get_length(decoder);

    UtObjectRef component_name = match_component(decoder, components);
    if (component_name == NULL) {
      // If extensible, ignore all unknown components.
      if (extensible) {
        continue;
      }
      set_error(self, "Unknown SET component");
      return ut_map_new();
    }

    if (ut_map_lookup(value, component_name)) {
      ut_cstring_ref description = ut_cstring_new_printf(
          "Duplicate SET component %s", ut_string_get_text(component_name));
      set_error(self, description);
      return ut_map_new();
    }

    UtObject *component_type = ut_map_lookup(components, component_name);
    if (ut_object_is_asn1_optional_type(component_type)) {
      component_type = ut_asn1_optional_type_get_type(component_type);
    } else if (ut_object_is_asn1_default_type(component_type)) {
      component_type = ut_asn1_default_type_get_type(component_type);
    } else {
      required_component_count++;
    }
    UtObjectRef component_value =
        ut_asn1_decoder_decode_value(decoder, component_type);
    UtObject *error = ut_asn1_decoder_get_error(decoder);
    if (error != NULL) {
      ut_cstring_ref child_description = ut_error_get_description(error);
      ut_cstring_ref description =
          ut_cstring_new_printf("Error decoding SET element %zi: %s",
                                ut_list_get_length(value), child_description);
      set_error(self, description);
      return ut_map_new();
    }

    ut_map_insert_string(value, ut_string_get_text(component_name),
                         component_value);
  }

  // Set any missing default components and check if we have all required
  // components.
  UtObjectRef component_items = ut_map_get_items(components);
  size_t total_required_components = 0;
  for (size_t i = 0; i < components_length; i++) {
    UtObject *item = ut_object_list_get_element(component_items, i);
    UtObjectRef type = ut_map_item_get_value(item);
    if (ut_object_is_asn1_default_type(type)) {
      UtObjectRef name = ut_map_item_get_key(item);
      if (ut_map_lookup_string(value, ut_string_get_text(name)) == NULL) {
        ut_map_insert_string(value, ut_string_get_text(name),
                             ut_asn1_default_type_get_default_value(type));
      }
    } else if (!ut_object_is_asn1_optional_type(type)) {
      total_required_components++;
    }
  }

  if (required_component_count < total_required_components) {
    // FIXME: List missing components.
    set_error(self, "Required SET components missing");
    return ut_map_new();
  }

  return ut_object_ref(value);
}

static UtObject *decode_set_of_value(UtAsn1BerDecoder *self, UtObject *type,
                                     bool decode_tag) {
  if (decode_tag && !expect_tag(self, UT_ASN1_TAG_CLASS_UNIVERSAL,
                                UT_ASN1_TAG_UNIVERSAL_SET)) {
    return ut_list_new();
  }
  if (!self->constructed) {
    set_error(self, "SET OF does not have primitive form");
    return ut_list_new();
  }

  return decode_value_list(self, ut_asn1_set_of_type_get_type(type), "SET OF");
}

static UtObject *decode_value(UtAsn1BerDecoder *self, UtObject *type,
                              bool decode_tag);

static UtObject *decode_choice_value(UtAsn1BerDecoder *self, UtObject *type,
                                     bool decode_tag) {
  UtObject *components = ut_asn1_choice_type_get_components(type);

  // FIXME: decode_tag must be true - throw an error?

  UtObjectRef items = ut_map_get_items(components);
  size_t items_length = ut_list_get_length(items);
  for (size_t i = 0; i < items_length; i++) {
    UtObject *item = ut_object_list_get_element(items, i);
    UtObjectRef item_type = ut_map_item_get_value(item);
    if (ut_asn1_type_matches_tag(item_type, self->tag)) {
      UtObjectRef identifier = ut_map_item_get_key(item);
      UtObjectRef value = decode_value(self, item_type, true);
      return ut_asn1_choice_value_new(ut_string_get_text(identifier), value);
    }
  }

  // Can have unknown values if extensible.
  if (ut_asn1_choice_type_get_extensible(type)) {
    return ut_asn1_choice_value_new("<extension>", NULL);
  }

  set_error(self, "Unknown CHOICE value");
  return ut_asn1_choice_value_new_take("", NULL);
}

static UtObject *decode_numeric_string_value(UtAsn1BerDecoder *self,
                                             bool decode_tag) {
  if (decode_tag && !expect_tag(self, UT_ASN1_TAG_CLASS_UNIVERSAL,
                                UT_ASN1_TAG_UNIVERSAL_NUMERIC_STRING)) {
    return ut_string_new("");
  }
  return decode_numeric_string(self);
}

static UtObject *decode_printable_string_value(UtAsn1BerDecoder *self,
                                               bool decode_tag) {
  if (decode_tag && !expect_tag(self, UT_ASN1_TAG_CLASS_UNIVERSAL,
                                UT_ASN1_TAG_UNIVERSAL_PRINTABLE_STRING)) {
    return ut_string_new("");
  }
  return decode_printable_string(self);
}

static UtObject *decode_ia5_string_value(UtAsn1BerDecoder *self,
                                         bool decode_tag) {
  if (decode_tag && !expect_tag(self, UT_ASN1_TAG_CLASS_UNIVERSAL,
                                UT_ASN1_TAG_UNIVERSAL_IA5_STRING)) {
    return ut_string_new("");
  }
  return decode_ia5_string(self);
}

static UtObject *decode_graphic_string_value(UtAsn1BerDecoder *self,
                                             bool decode_tag) {
  if (decode_tag && !expect_tag(self, UT_ASN1_TAG_CLASS_UNIVERSAL,
                                UT_ASN1_TAG_UNIVERSAL_GRAPHIC_STRING)) {
    return ut_string_new("");
  }
  return decode_graphic_string(self, "GraphicString");
}

static UtObject *decode_teletex_string_value(UtAsn1BerDecoder *self,
                                             bool decode_tag) {
  return ut_string_new("");
}

static UtObject *decode_videotex_string_value(UtAsn1BerDecoder *self,
                                              bool decode_tag) {
  return ut_string_new("");
}

static UtObject *decode_visible_string_value(UtAsn1BerDecoder *self,
                                             bool decode_tag) {
  if (decode_tag && !expect_tag(self, UT_ASN1_TAG_CLASS_UNIVERSAL,
                                UT_ASN1_TAG_UNIVERSAL_VISIBLE_STRING)) {
    return ut_string_new("");
  }
  return decode_visible_string(self);
}

static UtObject *decode_general_string_value(UtAsn1BerDecoder *self,
                                             bool decode_tag) {
  if (decode_tag && !expect_tag(self, UT_ASN1_TAG_CLASS_UNIVERSAL,
                                UT_ASN1_TAG_UNIVERSAL_GENERAL_STRING)) {
    return ut_string_new("");
  }
  return decode_general_string(self);
}

static UtObject *decode_tagged_value(UtAsn1BerDecoder *self, UtObject *type,
                                     bool decode_tag) {
  if (decode_tag && !expect_tag(self, ut_asn1_tagged_type_get_class(type),
                                ut_asn1_tagged_type_get_number(type))) {
    return NULL;
  }

  if (ut_asn1_tagged_type_get_is_explicit(type)) {
    UtObjectRef decoder = ut_asn1_ber_decoder_new(self->contents);
    return ut_asn1_decoder_decode_value(decoder,
                                        ut_asn1_tagged_type_get_type(type));
  } else {
    return decode_value(self, ut_asn1_tagged_type_get_type(type), false);
  }
}

static UtObject *decode_value(UtAsn1BerDecoder *self, UtObject *type,
                              bool decode_tag) {
  if (ut_object_is_asn1_boolean_type(type)) {
    return decode_boolean_value(self, decode_tag);
  } else if (ut_object_is_asn1_integer_type(type)) {
    return decode_integer_value(self, decode_tag);
  } else if (ut_object_is_asn1_bit_string_type(type)) {
    return decode_bit_string_value(self, decode_tag);
  } else if (ut_object_is_asn1_octet_string_type(type)) {
    return decode_octet_string_value(self, decode_tag);
  } else if (ut_object_is_asn1_null_type(type)) {
    return decode_null_value(self, decode_tag);
  } else if (ut_object_is_asn1_object_identifier_type(type)) {
    return decode_object_identifier_value(self, decode_tag);
  } else if (ut_object_is_asn1_object_descriptor_type(type)) {
    return decode_object_descriptor_value(self, decode_tag);
  } else if (ut_object_is_asn1_real_type(type)) {
    return decode_real_value(self, decode_tag);
  } else if (ut_object_is_asn1_enumerated_type(type)) {
    return decode_enumerated_value(self, type, decode_tag);
  } else if (ut_object_is_asn1_utf8_string_type(type)) {
    return decode_utf8_string_value(self, decode_tag);
  } else if (ut_object_is_asn1_relative_oid_type(type)) {
    return decode_relative_oid_value(self, decode_tag);
  } else if (ut_object_is_asn1_sequence_type(type)) {
    return decode_sequence_value(self, type, decode_tag);
  } else if (ut_object_is_asn1_sequence_of_type(type)) {
    return decode_sequence_of_value(self, type, decode_tag);
  } else if (ut_object_is_asn1_set_type(type)) {
    return decode_set_value(self, type, decode_tag);
  } else if (ut_object_is_asn1_set_of_type(type)) {
    return decode_set_of_value(self, type, decode_tag);
  } else if (ut_object_is_asn1_choice_type(type)) {
    return decode_choice_value(self, type, decode_tag);
  } else if (ut_object_is_asn1_numeric_string_type(type)) {
    return decode_numeric_string_value(self, decode_tag);
  } else if (ut_object_is_asn1_printable_string_type(type)) {
    return decode_printable_string_value(self, decode_tag);
  } else if (ut_object_is_asn1_ia5_string_type(type)) {
    return decode_ia5_string_value(self, decode_tag);
  } else if (ut_object_is_asn1_graphic_string_type(type)) {
    return decode_graphic_string_value(self, decode_tag);
  } else if (ut_object_is_asn1_visible_string_type(type)) {
    return decode_visible_string_value(self, decode_tag);
  } else if (ut_object_is_asn1_general_string_type(type)) {
    return decode_general_string_value(self, decode_tag);
  } else if (ut_object_is_asn1_teletex_string_type(type)) {
    return decode_teletex_string_value(self, decode_tag);
  } else if (ut_object_is_asn1_videotex_string_type(type)) {
    return decode_videotex_string_value(self, decode_tag);
  } else if (ut_object_is_asn1_tagged_type(type)) {
    return decode_tagged_value(self, type, decode_tag);
  } else {
    ut_cstring_ref description = ut_cstring_new_printf(
        "Unknown ASN.1 type %s", ut_object_get_type_name(type));
    set_error(self, description);
    return NULL;
  }
}

static UtObject *ut_asn1_ber_decoder_decode_value(UtObject *object,
                                                  UtObject *type) {
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  return decode_value(self, type, true);
}

static UtObject *ut_asn1_ber_decoder_get_error(UtObject *object) {
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  return self->error;
}

static char *ut_asn1_ber_decoder_to_string(UtObject *object) {
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  ut_cstring_ref contents_string = ut_object_to_string(self->contents);
  return ut_cstring_new_printf("<UtAsn1BerDecoder>(%s)", contents_string);
}

static void ut_asn1_ber_decoder_cleanup(UtObject *object) {
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  ut_object_unref(self->tag);
  ut_object_unref(self->contents);
  ut_object_unref(self->error);
}

static UtAsn1DecoderInterface asn1_decoder_interface = {
    .decode_value = ut_asn1_ber_decoder_decode_value,
    .get_error = ut_asn1_ber_decoder_get_error};

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1BerDecoder",
    .to_string = ut_asn1_ber_decoder_to_string,
    .cleanup = ut_asn1_ber_decoder_cleanup,
    .interfaces = {{&ut_asn1_decoder_id, &asn1_decoder_interface},
                   {NULL, NULL}}};

UtObject *ut_asn1_ber_decoder_new(UtObject *data) {
  UtObjectRef object =
      ut_object_new(sizeof(UtAsn1BerDecoder), &object_interface);
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;

  size_t data_length = ut_list_get_length(data);
  if (data_length == 0) {
    set_error(self, "Insufficient data for ASN.1 BER identifier");
    self->contents = ut_uint8_list_new();
    return ut_object_ref(object);
  }
  size_t offset = 0;
  uint8_t identifier = ut_uint8_list_get_element(data, offset);
  offset++;
  UtAsn1TagClass class;
  switch (identifier & 0xc0) {
  case 0x00:
    class = UT_ASN1_TAG_CLASS_UNIVERSAL;
    break;
  case 0x40:
    class = UT_ASN1_TAG_CLASS_APPLICATION;
    break;
  case 0x80:
    class = UT_ASN1_TAG_CLASS_CONTEXT_SPECIFIC;
    break;
  case 0xc0:
    class = UT_ASN1_TAG_CLASS_PRIVATE;
    break;
  }
  self->constructed = (identifier & 0x20) != 0;

  uint32_t number = identifier & 0x1f;
  if (number >= 0x1f) {
    if (!decode_compressed_integer(data, &offset, &number)) {
      set_error(self, "Insufficient data for ASN.1 BER identifier number");
      self->contents = ut_uint8_list_new();
      return ut_object_ref(object);
    }
  }
  self->tag = ut_asn1_tag_new(class, number);

  if (offset >= data_length) {
    set_error(self, "Insufficient data for ASN.1 BER length");
    self->contents = ut_uint8_list_new();
    return ut_object_ref(object);
  }
  size_t length = ut_uint8_list_get_element(data, offset);
  offset++;
  if ((length & 0x80) != 0) {
    size_t n_octets = length & 0x7f;
    length = 0;
    if (offset + n_octets > data_length) {
      set_error(self, "Insufficient data for ASN.1 BER length");
      self->contents = ut_uint8_list_new();
      return ut_object_ref(object);
    }

    if (n_octets == 0) {
      set_error(self, "Indefinite length not supported");
      self->contents = ut_uint8_list_new();
      return ut_object_ref(object);
    } else {
      if (n_octets > 4) {
        set_error(self, "Lengths more than 32 bits not supported");
        self->contents = ut_uint8_list_new();
        return ut_object_ref(object);
      }
      for (size_t i = 0; i < n_octets; i++) {
        length = length << 8 | ut_uint8_list_get_element(data, offset);
        offset++;
      }
    }
  }

  if (offset + length > data_length) {
    set_error(self, "Insufficient data");
    self->contents = ut_uint8_list_new();
    return ut_object_ref(object);
  }

  self->contents = ut_list_get_sublist(data, offset, length);
  offset += length;

  self->length = offset;

  return ut_object_ref(object);
}

UtObject *ut_asn1_ber_decoder_get_tag(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  return self->tag;
}

bool ut_asn1_ber_decoder_get_constructed(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  return self->constructed;
}

size_t ut_asn1_ber_decoder_get_length(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  return self->length;
}

UtObject *ut_asn1_ber_decoder_get_contents(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  return self->contents;
}

bool ut_asn1_ber_decoder_decode_boolean(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  return decode_boolean(self);
}

int64_t ut_asn1_ber_decoder_decode_integer(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  return decode_integer(self, "INTEGER");
}

UtObject *ut_asn1_ber_decoder_decode_bit_string(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  return decode_bit_string(self);
}

UtObject *ut_asn1_ber_decoder_decode_octet_string(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  return decode_octet_string(self, "OCTET STRING");
}

void ut_asn1_ber_decoder_decode_null(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  decode_null(self);
}

UtObject *ut_asn1_ber_decoder_decode_object_identifier(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  return decode_object_identifier(self);
}

char *ut_asn1_ber_decoder_decode_object_descriptor(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  UtObjectRef value = decode_object_descriptor(self);
  return ut_string_take_text(value);
}

double ut_asn1_ber_decoder_decode_real(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  return decode_real(self);
}

int64_t ut_asn1_ber_decoder_decode_enumerated(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  return decode_enumerated(self);
}

char *ut_asn1_ber_decoder_decode_utf8_string(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  UtObjectRef value = decode_utf8_string(self);
  return ut_string_take_text(value);
}

UtObject *ut_asn1_ber_decoder_decode_relative_oid(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  return decode_relative_oid(self);
}

UtObject *ut_asn1_ber_decoder_decode_sequence(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;

  if (!self->constructed) {
    set_error(self, "Sequence must be constructed");
    return ut_list_new();
  }

  return decode_constructed(self);
}

UtObject *ut_asn1_ber_decoder_decode_set(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;

  UtObjectRef set = ut_list_new();

  if (!self->constructed) {
    set_error(self, "Set must be constructed");
    return ut_list_new();
  }

  return decode_constructed(self);
}

char *ut_asn1_ber_decoder_decode_numeric_string(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  UtObjectRef value = decode_numeric_string(self);
  return ut_string_take_text(value);
}

char *ut_asn1_ber_decoder_decode_printable_string(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  UtObjectRef value = decode_printable_string(self);
  return ut_string_take_text(value);
}

char *ut_asn1_ber_decoder_decode_ia5_string(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  UtObjectRef value = decode_ia5_string(self);
  return ut_string_take_text(value);
}

char *ut_asn1_ber_decoder_decode_graphic_string(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  UtObjectRef value = decode_graphic_string(self, "GraphicString");
  return ut_string_take_text(value);
}

char *ut_asn1_ber_decoder_decode_visible_string(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  UtObjectRef value = decode_visible_string(self);
  return ut_string_take_text(value);
}

char *ut_asn1_ber_decoder_decode_general_string(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  UtObjectRef value = decode_general_string(self);
  return ut_string_take_text(value);
}

bool ut_object_is_asn1_ber_decoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
