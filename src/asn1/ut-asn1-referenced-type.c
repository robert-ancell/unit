#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *type;
} UtAsn1ReferencedType;

static UtObject *ut_asn1_referenced_type_get_tags(UtObject *object) {
  UtAsn1ReferencedType *self = (UtAsn1ReferencedType *)object;
  return ut_asn1_type_get_tags(self->type);
}

static bool ut_asn1_referenced_type_matches_tag(UtObject *object,
                                                UtObject *tag) {
  UtAsn1ReferencedType *self = (UtAsn1ReferencedType *)object;
  return ut_asn1_type_matches_tag(self->type, tag);
}

static UtObject *ut_asn1_referenced_type_get_base_type(UtObject *object) {
  UtAsn1ReferencedType *self = (UtAsn1ReferencedType *)object;
  return self->type;
}

static UtAsn1TypeInterface asn1_type_interface = {
    .get_tags = ut_asn1_referenced_type_get_tags,
    .matches_tag = ut_asn1_referenced_type_matches_tag,
    .get_base_type = ut_asn1_referenced_type_get_base_type};

static char *ut_asn1_referenced_type_to_string(UtObject *object) {
  UtAsn1ReferencedType *self = (UtAsn1ReferencedType *)object;
  ut_cstring_ref type_string = ut_object_to_string(self->type);
  return ut_cstring_new_printf("<UtAsn1ReferencedType>(%s)", type_string);
}

static bool ut_asn1_referenced_type_equal(UtObject *object, UtObject *other) {
  UtAsn1ReferencedType *self = (UtAsn1ReferencedType *)object;
  UtAsn1ReferencedType *other_self = (UtAsn1ReferencedType *)other;
  return ut_object_equal(self->type, ut_object_is_asn1_referenced_type(other)
                                         ? other_self->type
                                         : other);
}

static void ut_asn1_referenced_type_cleanup(UtObject *object) {
  UtAsn1ReferencedType *self = (UtAsn1ReferencedType *)object;
  ut_object_unref(self->type);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1ReferencedType",
    .to_string = ut_asn1_referenced_type_to_string,
    .equal = ut_asn1_referenced_type_equal,
    .cleanup = ut_asn1_referenced_type_cleanup,
    .interfaces = {{&ut_asn1_type_id, &asn1_type_interface}, {NULL, NULL}}};

UtObject *ut_asn1_referenced_type_new(UtObject *type) {
  UtObject *object =
      ut_object_new(sizeof(UtAsn1ReferencedType), &object_interface);
  UtAsn1ReferencedType *self = (UtAsn1ReferencedType *)object;

  self->type = ut_object_ref(type);

  return object;
}

UtObject *ut_asn1_referenced_type_get_type(UtObject *object) {
  assert(ut_object_is_asn1_referenced_type(object));
  UtAsn1ReferencedType *self = (UtAsn1ReferencedType *)object;
  return self->type;
}

bool ut_object_is_asn1_referenced_type(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
