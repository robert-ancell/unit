#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
} UtAsn1RealType;

static UtObject *ut_asn1_real_type_get_tags(UtObject *object) {
  return ut_list_new_from_elements_take(
      ut_asn1_tag_new_universal(UT_ASN1_TAG_UNIVERSAL_REAL), NULL);
}

static bool ut_asn1_real_type_matches_tag(UtObject *object, UtObject *tag) {
  return ut_asn1_tag_matches(tag, UT_ASN1_TAG_CLASS_UNIVERSAL,
                             UT_ASN1_TAG_UNIVERSAL_REAL);
}

static UtAsn1TypeInterface asn1_type_interface = {
    .get_tags = ut_asn1_real_type_get_tags,
    .matches_tag = ut_asn1_real_type_matches_tag};

static bool ut_asn1_real_type_equal(UtObject *object, UtObject *other) {
  return ut_object_is_asn1_real_type(other);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1RealType",
    .equal = ut_asn1_real_type_equal,
    .interfaces = {{&ut_asn1_type_id, &asn1_type_interface}, {NULL, NULL}}};

UtObject *ut_asn1_real_type_new() {
  return ut_object_new(sizeof(UtAsn1RealType), &object_interface);
}

bool ut_object_is_asn1_real_type(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
