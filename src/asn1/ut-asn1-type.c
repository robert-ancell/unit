#include <assert.h>

#include "ut.h"

int ut_asn1_type_id = 0;

UtObject *ut_asn1_type_get_tags(UtObject *object) {
  UtAsn1TypeInterface *asn1_type_interface =
      ut_object_get_interface(object, &ut_asn1_type_id);
  assert(asn1_type_interface != NULL);
  return asn1_type_interface->get_tags(object);
}

bool ut_object_implements_asn1_type(UtObject *object) {
  return ut_object_get_interface(object, &ut_asn1_type_id) != NULL;
}
