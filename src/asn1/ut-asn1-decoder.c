#include <assert.h>

#include "ut.h"

int ut_asn1_decoder_id = 0;

UtObject *ut_asn1_decoder_decode_value(UtObject *object, UtObject *type) {
  UtAsn1DecoderInterface *asn1_decoder_interface =
      ut_object_get_interface(object, &ut_asn1_decoder_id);
  assert(asn1_decoder_interface != NULL);
  return asn1_decoder_interface->decode_value(object, type);
}

UtObject *ut_asn1_decoder_get_error(UtObject *object) {
  UtAsn1DecoderInterface *asn1_decoder_interface =
      ut_object_get_interface(object, &ut_asn1_decoder_id);
  assert(asn1_decoder_interface != NULL);
  return asn1_decoder_interface->get_error(object);
}

bool ut_object_implements_asn1_decoder(UtObject *object) {
  return ut_object_get_interface(object, &ut_asn1_decoder_id) != NULL;
}
