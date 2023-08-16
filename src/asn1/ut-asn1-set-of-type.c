#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *type;
} UtAsn1SetOfType;

static UtObject *ut_asn1_set_of_type_get_tags(UtObject *object) {
  return ut_list_new_from_elements_take(
      ut_asn1_tag_new_universal(UT_ASN1_TAG_UNIVERSAL_SET), NULL);
}

static UtAsn1TypeInterface asn1_type_interface = {
    .get_tags = ut_asn1_set_of_type_get_tags};

static char *ut_asn1_set_of_type_to_string(UtObject *object) {
  UtAsn1SetOfType *self = (UtAsn1SetOfType *)object;
  ut_cstring_ref type_string = ut_object_to_string(self->type);
  return ut_cstring_new_printf("<UtAsn1SetOfType>(%s)", type_string);
}

static void ut_asn1_set_of_type_cleanup(UtObject *object) {
  UtAsn1SetOfType *self = (UtAsn1SetOfType *)object;
  ut_object_unref(self->type);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1SetOfType",
    .to_string = ut_asn1_set_of_type_to_string,
    .cleanup = ut_asn1_set_of_type_cleanup,
    .interfaces = {{&ut_asn1_type_id, &asn1_type_interface}, {NULL, NULL}}};

UtObject *ut_asn1_set_of_type_new(UtObject *type) {
  UtObject *object = ut_object_new(sizeof(UtAsn1SetOfType), &object_interface);
  UtAsn1SetOfType *self = (UtAsn1SetOfType *)object;

  self->type = ut_object_ref(type);

  return object;
}

UtObject *ut_asn1_set_of_type_get_type(UtObject *object) {
  assert(ut_object_is_asn1_set_of_type(object));
  UtAsn1SetOfType *self = (UtAsn1SetOfType *)object;
  return self->type;
}

bool ut_object_is_asn1_set_of_type(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
