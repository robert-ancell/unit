#include <assert.h>

#include "ut-asn1-string.h"
#include "ut.h"

typedef struct {
  UtObject object;

  // Identifier fields.
  uint32_t number;
  UtAsn1TagClass class;
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
    UtObject *child_error = ut_asn1_ber_decoder_get_error(child);
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

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1BerDecoder",
    .to_string = ut_asn1_ber_decoder_to_string,
    .cleanup = ut_asn1_ber_decoder_cleanup};

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

UtObject *ut_asn1_ber_decoder_get_error(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  return self->error;
}

bool ut_asn1_ber_decoder_decode_boolean(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;

  size_t data_length = ut_list_get_length(self->contents);
  if (data_length != 1) {
    set_error(self, "Invalid boolean data length");
    return false;
  }
  if (self->constructed) {
    set_error(self, "Boolean does not have constructed form");
    return false;
  }
  bool value = ut_uint8_list_get_element(self->contents, 0) != 0;

  return value;
}

int64_t ut_asn1_ber_decoder_decode_integer(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;

  size_t data_length = ut_list_get_length(self->contents);
  if (data_length == 0) {
    set_error(self, "Invalid integer data length");
    return 0;
  }
  if (data_length > 8) {
    set_error(self, "Only 64 bit integers supported");
    return 0;
  }
  if (self->constructed) {
    set_error(self, "Integer does not have constructed form");
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

UtObject *ut_asn1_ber_decoder_decode_octet_string(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  if (self->constructed) {
    set_error(self, "Constructed octet string not supported");
    return false;
  }
  return ut_object_ref(self->contents);
}

void ut_asn1_ber_decoder_decode_null(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;

  size_t data_length = ut_list_get_length(self->contents);
  if (data_length != 0) {
    set_error(self, "Invalid null data length");
    return;
  }
  if (self->constructed) {
    set_error(self, "Null does not have constructed form");
    return;
  }
}

UtObject *ut_asn1_ber_decoder_decode_object_identifier(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;

  size_t data_length = ut_list_get_length(self->contents);
  UtObjectRef identifier = ut_uint32_list_new();

  if (self->constructed) {
    set_error(self, "Object identifier does not have constructed form");
    return ut_object_ref(identifier);
  }

  size_t offset = 0;
  uint32_t subidentifier0;
  if (!decode_compressed_integer(self->contents, &offset, &subidentifier0)) {
    set_error(self, "Invalid object identifier");
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
      set_error(self, "Invalid object identifier");
      return ut_object_ref(identifier);
    }
    ut_uint32_list_append(identifier, subidentifier);
  }
  return ut_object_ref(identifier);
}

int64_t ut_asn1_ber_decoder_decode_enumerated(UtObject *object) {
  // FIXME: Errors will refer to integer
  return ut_asn1_ber_decoder_decode_integer(object);
}

char *ut_asn1_ber_decoder_decode_utf8_string(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;

  if (self->constructed) {
    set_error(self, "Constructed utf8 string not supported");
    return ut_cstring_new("");
  }

  UtObjectRef value = ut_string_new_from_utf8(self->contents);
  if (ut_object_implements_error(value)) {
    set_error(self, "Invalid UTF-8 string");
    return ut_cstring_new("");
  }

  return ut_string_take_text(value);
}

UtObject *ut_asn1_ber_decoder_decode_relative_oid(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;

  size_t data_length = ut_list_get_length(self->contents);
  UtObjectRef identifier = ut_uint32_list_new();

  if (self->constructed) {
    set_error(self,
              "Relative object identifier does not have constructed form");
    return ut_object_ref(identifier);
  }

  size_t offset = 0;
  while (offset < data_length) {
    uint32_t subidentifier;
    if (!decode_compressed_integer(self->contents, &offset, &subidentifier)) {
      set_error(self, "Invalid relative object identifier");
      return ut_object_ref(identifier);
    }
    ut_uint32_list_append(identifier, subidentifier);
  }
  return ut_object_ref(identifier);
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
