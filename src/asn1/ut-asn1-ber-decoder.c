#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtAsn1TagClass class;
  bool constructed;
  size_t number;
  UtObject *contents;
} UtAsn1BerDecoder;

static void ut_asn1_ber_decoder_cleanup(UtObject *object) {
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  ut_object_unref(self->contents);
}

static UtObjectInterface object_interface = {.type_name = "UtAsn1BerDecoder",
                                             .cleanup =
                                                 ut_asn1_ber_decoder_cleanup,
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_asn1_ber_decoder_new(UtObject *data) {
  UtObjectRef object =
      ut_object_new(sizeof(UtAsn1BerDecoder), &object_interface);
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;

  size_t data_length = ut_list_get_length(data);
  size_t offset = 0;
  if (offset >= data_length) {
    return ut_error_new("Empty ASN.1 data");
  }
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

  self->number = identifier & 0x1f;
  if (self->number == 0x1f) {
    // Long form
    self->number = 0;
    uint8_t number_octet;
    do {
      if (offset >= data_length) {
        return ut_error_new("Invalid identifier");
      }
      number_octet = ut_uint8_list_get_element(data, offset);
      offset++;
      self->number = self->number << 7 | (number_octet & 0x7f);
    } while ((number_octet & 0x80) != 0);
  }

  if (offset >= data_length) {
    return ut_error_new("Invalid length");
  }
  size_t contents_length = ut_uint8_list_get_element(data, offset);
  offset++;
  if ((contents_length & 0x80) != 0) {
    size_t n_octets = contents_length & 0x7f;
    contents_length = 0;
    if (offset + n_octets > data_length) {
      return ut_error_new("Invalid length");
    }
    if (n_octets == 0) {
      // FIXME: Indefinite length, need to read tags to calculate length
    } else {
      for (size_t i = 0; i < n_octets; i++) {
        contents_length =
            contents_length << 8 | ut_uint8_list_get_element(data, offset);
        offset++;
      }
    }

    if (offset + contents_length != data_length) {
      return ut_error_new("Invalid length");
    }
    self->contents = ut_list_get_sublist(data, offset, contents_length);
  }

  return ut_object_ref(object);
}

UtAsn1TagClass ut_asn1_ber_decoder_get_class(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  return self->class;
}

bool ut_asn1_ber_decoder_get_constructed(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  return self->constructed;
}

unsigned int ut_asn1_ber_decoder_get_number(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  return self->number;
}

UtObject *ut_asn1_ber_decoder_get_contents(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  return self->contents;
}

bool ut_asn1_ber_decoder_decode_boolean(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  assert(ut_list_get_length(self->contents) == 1);
  return ut_uint8_list_get_element(self->contents, 0) != 0;
}

int64_t ut_asn1_ber_decoder_decode_integer(UtObject *object) {
  assert(false);
  return 0;
}

UtObject *ut_asn1_ber_decoder_decode_octet_string(UtObject *object) {
  assert(ut_object_is_asn1_ber_decoder(object));
  UtAsn1BerDecoder *self = (UtAsn1BerDecoder *)object;
  return ut_object_ref(self->contents);
}

char *ut_asn1_ber_decoder_decode_utf8_string(UtObject *object) {
  assert(false);
  return ut_cstring_new("");
}

bool ut_object_is_asn1_ber_decoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
