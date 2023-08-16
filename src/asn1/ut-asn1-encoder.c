#include <assert.h>

#include "ut.h"

int ut_asn1_encoder_id = 0;

void ut_asn1_encoder_encode_value(UtObject *object, UtObject *type,
                                  UtObject *value) {
  UtAsn1EncoderInterface *asn1_encoder_interface =
      ut_object_get_interface(object, &ut_asn1_encoder_id);
  assert(asn1_encoder_interface != NULL);
  asn1_encoder_interface->encode_value(object, type, value);
}

UtObject *ut_asn1_encoder_get_error(UtObject *object) {
  UtAsn1EncoderInterface *asn1_encoder_interface =
      ut_object_get_interface(object, &ut_asn1_encoder_id);
  assert(asn1_encoder_interface != NULL);
  return asn1_encoder_interface->get_error(object);
}

bool ut_object_implements_asn1_encoder(UtObject *object) {
  return ut_object_get_interface(object, &ut_asn1_encoder_id) != NULL;
}
