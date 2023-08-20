#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
} UtAsn1RelativeOidType;

static UtObject *ut_asn1_relative_oid_type_get_tags(UtObject *object) {
  return ut_list_new_from_elements_take(
      ut_asn1_tag_new_universal(UT_ASN1_TAG_UNIVERSAL_RELATIVE_OID), NULL);
}

static UtAsn1TypeInterface asn1_type_interface = {
    .get_tags = ut_asn1_relative_oid_type_get_tags};

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1RelativeOidType",
    .interfaces = {{&ut_asn1_type_id, &asn1_type_interface}, {NULL, NULL}}};

UtObject *ut_asn1_relative_oid_type_new() {
  return ut_object_new(sizeof(UtAsn1RelativeOidType), &object_interface);
}

bool ut_object_is_asn1_relative_oid_type(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}