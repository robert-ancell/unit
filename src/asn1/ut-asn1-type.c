#include <assert.h>

#include "ut.h"

int ut_asn1_type_id = 0;

UtObject *ut_asn1_type_get_tags(UtObject *object) {
  UtAsn1TypeInterface *asn1_type_interface =
      ut_object_get_interface(object, &ut_asn1_type_id);
  assert(asn1_type_interface != NULL);
  return asn1_type_interface->get_tags(object);
}

bool ut_asn1_type_matches_tag(UtObject *object, UtObject *tag) {
  UtAsn1TypeInterface *asn1_type_interface =
      ut_object_get_interface(object, &ut_asn1_type_id);
  assert(asn1_type_interface != NULL);
  return asn1_type_interface->matches_tag(object, tag);
}

UtObject *ut_asn1_type_get_base_type(UtObject *object) {
  UtAsn1TypeInterface *asn1_type_interface =
      ut_object_get_interface(object, &ut_asn1_type_id);
  assert(asn1_type_interface != NULL);
  if (asn1_type_interface->get_base_type) {
    return asn1_type_interface->get_base_type(object);
  } else {
    return object;
  }
}

bool ut_object_implements_asn1_type(UtObject *object) {
  return ut_object_get_interface(object, &ut_asn1_type_id) != NULL;
}
