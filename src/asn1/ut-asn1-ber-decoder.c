#include <assert.h>

#include "ut-asn1-string.h"
#include "ut.h"

typedef struct {
  UtObject object;

  // Identifier fields.
  UtAsn1TagClass class;
  uint32_t number;
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

static int64_t decode_integer(UtAsn1BerDecoder *self) {
  size_t data_length = ut_list_get_length(self->contents);
  if (data_length == 0) {
    set_error(self, "Invalid INTEGER data length");
    return 0;
  }
  if (data_length > 8) {
    set_error(self, "Only 64 bit integers supported");
    return 0;
  }
  if (self->constructed) {
    set_error(self, "INTEGER does not have constructed form");
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

static UtObject *decode_bit_string(UtAsn1BerDecoder *self) {
  size_t data_length = ut_list_get_length(self->contents);
  if (data_length < 1) {
    set_error(self, "Invalid BIT STRING data length");
    return ut_bit_list_new_msb();
  }
  if (self->constructed) {
    set_error(self, "Constructed BIT STRING not supported");
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

    if (ut_asn1_ber_decoder_get_tag_class(decoder) != self->class ||
        ut_asn1_ber_decoder_get_identifier_number(decoder) != self->number) {
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

static double decode_real(UtAsn1BerDecoder *self) {
  if (self->constructed) {
    set_error(self, "REAL does not have constructed form");
    return 0.0;
  }

  // FIXME
  assert(false);
  return 0.0;
}

static int64_t decode_enumerated(UtAsn1BerDecoder *self) {
  if (self->constructed) {
    set_error(self, "ENUMERATED does not have constructed form");
    return 0;
  }

  // FIXME: Errors will refer to integer
  return decode_integer(self);
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
  if (self->class != class || self->number != number) {
    set_error(self, "Unexpected tag");
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
  return ut_int64_new(decode_integer(self));
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

static bool match_type(UtObject *decoder, UtObject *type) {
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)decoder;

  // FIXME: Inefficient - most types will be a single tag.
  // Make ut_asn1_type_matches_tag?
  UtObjectRef tags = ut_asn1_type_get_tags(type);
  size_t tags_length = ut_list_get_length(tags);
  for (size_t i = 0; i < tags_length; i++) {
    UtObject *tag = ut_object_list_get_element(tags, i);
    if (ut_asn1_tag_get_class(tag) == self->class &&
        ut_asn1_tag_get_number(tag) == self->number) {
      return true;
    }
  }

  return false;
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

  UtObject *components = ut_asn1_sequence_type_get_components(type);
  size_t components_length = ut_map_get_length(components);
  UtObjectRef component_items = ut_map_get_items(components);
  size_t next_component = 0;

  UtObjectRef value = ut_map_new();
  size_t contents_length = ut_list_get_length(self->contents);
  size_t offset = 0;
  while (offset < contents_length) {
    UtObjectRef data =
        ut_list_get_sublist(self->contents, offset, contents_length - offset);
    UtObjectRef decoder = ut_asn1_ber_decoder_new(data);
    offset += ut_asn1_ber_decoder_get_length(decoder);

    if (next_component >= components_length) {
      // FIXME: Ignore if sequence extensible
      set_error(self, "Required SEQUENCE components missing");
      return ut_map_new();
    }

    UtObjectRef item = ut_list_get_element(component_items, next_component);
    UtObjectRef component_name = ut_map_item_get_key(item);
    UtObjectRef component_type = ut_map_item_get_value(item);
    if (!match_type(decoder, component_type)) {
      ut_cstring_ref description =
          ut_cstring_new_printf("Required SEQUENCE component %s missing",
                                ut_string_get_text(component_name));
      set_error(self, description);
      return ut_map_new();
    }
    next_component++;
    UtObjectRef component_value =
        ut_asn1_decoder_decode_value(decoder, component_type);
    UtObject *error = ut_asn1_decoder_get_error(decoder);
    if (error != NULL) {
      ut_cstring_ref child_description = ut_error_get_description(error);
      ut_cstring_ref description =
          ut_cstring_new_printf("Error decoding SEQUENCE element %zi: %s",
                                ut_list_get_length(value), child_description);
      set_error(self, description);
      return ut_map_new();
    }

    ut_map_insert_string(value, ut_string_get_text(component_name),
                         component_value);
  }

  if (next_component < components_length) {
    // FIXME: List missing components
    // FIXME: Ignore if only optional components remain
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

  size_t components_length = ut_map_get_length(components);
  for (size_t i = 0; i < components_length; i++) {
    UtObject *item = ut_object_list_get_element(items, i);
    UtObjectRef item_type = ut_map_item_get_value(item);
    if (match_type(decoder, item_type)) {
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

  UtObject *components = ut_asn1_set_type_get_components(type);
  size_t components_length = ut_map_get_length(components);

  UtObjectRef value = ut_map_new();
  size_t contents_length = ut_list_get_length(self->contents);
  size_t offset = 0;
  while (offset < contents_length) {
    UtObjectRef data =
        ut_list_get_sublist(self->contents, offset, contents_length - offset);
    UtObjectRef decoder = ut_asn1_ber_decoder_new(data);
    offset += ut_asn1_ber_decoder_get_length(decoder);

    UtObjectRef component_name = match_component(decoder, components);
    if (component_name == NULL) {
      // FIXME: Allowed if extensible.
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

  if (ut_map_get_length(value) < components_length) {
    // FIXME: List missing components
    // FIXME: Ignore if only optional components remain
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
  switch (identifier & 0xc0) {
  case 0x00:
    self->class = UT_ASN1_TAG_CLASS_UNIVERSAL;
    break;
  case 0x40:
    self->class = UT_ASN1_TAG_CLASS_CONTEXT_SPECIFIC;
    break;
  case 0x80:
    self->class = UT_ASN1_TAG_CLASS_APPLICATION;
    break;
  case 0xc0:
    self->class = UT_ASN1_TAG_CLASS_PRIVATE;
    break;
  }
  self->constructed = (identifier & 0x20) != 0;

  uint32_t number = identifier & 0x1f;
  if (number < 0x1f) {
    self->number = number;
  } else {
    if (!decode_compressed_integer(data, &offset, &number)) {
      set_error(self, "Insufficient data for ASN.1 BER identifier number");
      self->contents = ut_uint8_list_new();
      return ut_object_ref(object);
    }
    self->number = number;
  }

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

uint32_t ut_asn1_ber_decoder_get_identifier_number(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  return self->number;
}

UtAsn1TagClass ut_asn1_ber_decoder_get_tag_class(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  return self->class;
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
  return decode_integer(self);
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

  if (self->constructed) {
    set_error(self, "Constructed numeric string not supported");
    return ut_cstring_new("");
  }

  UtObjectRef string = ut_asn1_decode_numeric_string(self->contents);
  if (string == NULL) {
    set_error(self, "Invalid numeric string");
    return ut_cstring_new("");
  }
  return ut_string_take_text(string);
}

char *ut_asn1_ber_decoder_decode_printable_string(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;

  if (self->constructed) {
    set_error(self, "Constructed printable string not supported");
    return ut_cstring_new("");
  }

  UtObjectRef string = ut_asn1_decode_printable_string(self->contents);
  if (string == NULL) {
    set_error(self, "Invalid printable string");
    return ut_cstring_new("");
  }
  return ut_string_take_text(string);
}

UtObject *ut_asn1_ber_decoder_decode_ia5_string(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;

  if (self->constructed) {
    set_error(self, "Constructed IA5 string not supported");
    return ut_string_new("");
  }

  UtObjectRef string = ut_asn1_decode_ia5_string(self->contents);
  if (string == NULL) {
    set_error(self, "Invalid IA5 string");
    return ut_string_new("");
  }
  return ut_object_ref(string);
}

char *ut_asn1_ber_decoder_decode_visible_string(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;

  if (self->constructed) {
    set_error(self, "Constructed visible string not supported");
    return ut_cstring_new("");
  }

  UtObjectRef string = ut_asn1_decode_visible_string(self->contents);
  if (string == NULL) {
    set_error(self, "Invalid visible string");
    return ut_cstring_new("");
  }
  return ut_string_take_text(string);
}

bool ut_object_is_asn1_ber_decoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
