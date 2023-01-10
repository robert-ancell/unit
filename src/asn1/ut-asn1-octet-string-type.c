#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
} UtAsn1OctetStringType;

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1OctetStringType", .interfaces = {{NULL, NULL}}};

UtObject *ut_asn1_octet_string_type_new() {
  return ut_object_new(sizeof(UtAsn1OctetStringType), &object_interface);
}

bool ut_object_is_asn1_octet_string_type(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
