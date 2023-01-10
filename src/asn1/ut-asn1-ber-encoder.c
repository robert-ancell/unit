#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *data;
} UtAsn1BerEncoder;

static void encode_identifier(UtAsn1BerEncoder *self, UtAsn1TagClass class,
                              bool constructed, unsigned int number) {
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
    octet0 |= number;
    ut_uint8_list_append(self->data, octet0);
  } else {
    // Long form
    octet0 |= 0x1f;
    ut_uint8_list_append(self->data, octet0);
    if (number <= 0x7f) {
      ut_uint8_list_append(self->data, number);
    } else if (number <= 0x3fff) {
      ut_uint8_list_append(self->data, number >> 7);
      ut_uint8_list_append(self->data, 0x80 | (number & 0x7f));
    } else if (number <= 0x1fffff) {
      ut_uint8_list_append(self->data, number >> 14);
      ut_uint8_list_append(self->data, (number >> 7) & 0x7f);
      ut_uint8_list_append(self->data, 0x80 | (number & 0x7f));
    } else {
      assert(false);
    }
  }
}

static void encode_definite_length(UtAsn1BerEncoder *self, size_t length) {
  if (length <= 0x7f) {
    ut_uint8_list_append(self->data, length);
  } else if (length <= 0xffff) {
    ut_uint8_list_append(self->data, 0x80 | 2);
    ut_uint8_list_append(self->data, length >> 8);
    ut_uint8_list_append(self->data, length & 0xff);
  } else if (length <= 0xffffff) {
    ut_uint8_list_append(self->data, 0x80 | 3);
    ut_uint8_list_append(self->data, length >> 16);
    ut_uint8_list_append(self->data, (length >> 8) & 0xff);
    ut_uint8_list_append(self->data, length & 0xff);
  } else if (length <= 0xffffffff) {
    ut_uint8_list_append(self->data, 0x80 | 4);
    ut_uint8_list_append(self->data, length >> 24);
    ut_uint8_list_append(self->data, (length >> 16) & 0xff);
    ut_uint8_list_append(self->data, (length >> 8) & 0xff);
    ut_uint8_list_append(self->data, length & 0xff);
  } else {
    assert(false);
  }
}

void ut_asn1_ber_encoder_init(UtObject *object) {
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;
  self->data = ut_uint8_array_new();
}

void ut_asn1_ber_encoder_cleanup(UtObject *object) {
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;
  ut_object_unref(self->data);
}

static UtObjectInterface object_interface = {.type_name = "UtAsn1BerEncoder",
                                             .init = ut_asn1_ber_encoder_init,
                                             .cleanup =
                                                 ut_asn1_ber_encoder_cleanup,
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_asn1_ber_encoder_new() {
  return ut_object_new(sizeof(UtAsn1BerEncoder), &object_interface);
}

void ut_asn1_ber_encoder_encode_primitive_identifier(UtObject *object,
                                                     UtAsn1TagClass class,
                                                     unsigned int number) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;

  encode_identifier(self, class, false, number);
}

void ut_asn1_ber_encoder_encode_constructed_identifier(UtObject *object,
                                                       UtAsn1TagClass class,
                                                       unsigned int number,
                                                       size_t length) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;

  encode_identifier(self, class, true, number);
  encode_definite_length(self, length);
}

void ut_asn1_ber_encoder_encode_constructed_identifier_indefinite_length(
    UtObject *object, UtAsn1TagClass class, unsigned int number) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;

  encode_identifier(self, class, true, number);
  ut_uint8_list_append(self->data, 0x80);
}

void ut_asn1_ber_encoder_encode_end_of_contents(UtObject *object) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;

  ut_uint8_list_append(self->data, 0x00);
  ut_uint8_list_append(self->data, 0x00);
}

void ut_asn1_ber_encoder_encode_boolean(UtObject *object, bool value) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;
  encode_definite_length(self, 1);
  ut_uint8_list_append(self->data, value ? 0xff : 0x00);
}

void ut_asn1_ber_encoder_encode_integer(UtObject *object, int64_t value) {
  assert(false);
}

void ut_asn1_ber_encoder_encode_octet_string(UtObject *object,
                                             UtObject *value) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;
  encode_definite_length(self, ut_list_get_length(value));
  ut_list_append_list(self->data, value);
}

void ut_asn1_ber_encoder_encode_utf8_string(UtObject *object,
                                            const char *value) {
  assert(false);
}

UtObject *ut_asn1_ber_encoder_get_data(UtObject *object) {
  assert(ut_object_is_asn1_ber_encoder(object));
  UtAsn1BerEncoder *self = (UtAsn1BerEncoder *)object;
  return self->data;
}

bool ut_object_is_asn1_ber_encoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
