#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
} UtAsn1EmbeddedIdentificationFixed;

static char *ut_asn1_embedded_identification_fixed_to_string(UtObject *object) {
  return ut_cstring_new("<UtAsn1EmbeddedIdentificationFixed>()");
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1EmbeddedIdentificationFixed",
    .to_string = ut_asn1_embedded_identification_fixed_to_string};

UtObject *ut_asn1_embedded_identification_fixed_new() {
  return ut_object_new(sizeof(UtAsn1EmbeddedIdentificationFixed),
                       &object_interface);
}

bool ut_object_is_asn1_embedded_identification_fixed(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
