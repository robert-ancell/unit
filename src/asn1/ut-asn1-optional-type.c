#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *type;
} UtAsn1OptionalType;

static UtObject *ut_asn1_optional_type_get_tags(UtObject *object) {
  UtAsn1OptionalType *self = (UtAsn1OptionalType *)object;
  return ut_asn1_type_get_tags(self->type);
}

static bool ut_asn1_optional_type_matches_tag(UtObject *object, UtObject *tag) {
  UtAsn1OptionalType *self = (UtAsn1OptionalType *)object;
  return ut_asn1_type_matches_tag(self->type, tag);
}

static UtAsn1TypeInterface asn1_type_interface = {
    .get_tags = ut_asn1_optional_type_get_tags,
    .matches_tag = ut_asn1_optional_type_matches_tag};

static char *ut_asn1_optional_type_to_string(UtObject *object) {
  UtAsn1OptionalType *self = (UtAsn1OptionalType *)object;
  ut_cstring_ref type_string = ut_object_to_string(self->type);
  return ut_cstring_new_printf("<UtAsn1OptionalType>(%s)", type_string);
}

static void ut_asn1_optional_type_cleanup(UtObject *object) {
  UtAsn1OptionalType *self = (UtAsn1OptionalType *)object;
  ut_object_unref(self->type);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1OptionalType",
    .to_string = ut_asn1_optional_type_to_string,
    .cleanup = ut_asn1_optional_type_cleanup,
    .interfaces = {{&ut_asn1_type_id, &asn1_type_interface}, {NULL, NULL}}};

UtObject *ut_asn1_optional_type_new(UtObject *type) {
  UtObject *object =
      ut_object_new(sizeof(UtAsn1OptionalType), &object_interface);
  UtAsn1OptionalType *self = (UtAsn1OptionalType *)object;

  self->type = ut_object_ref(type);

  return object;
}

UtObject *ut_asn1_optional_type_new_take(UtObject *type) {
  UtObject *object = ut_asn1_optional_type_new(type);
  ut_object_unref(type);
  return object;
}

UtObject *ut_asn1_optional_type_get_type(UtObject *object) {
  assert(ut_object_is_asn1_optional_type(object));
  UtAsn1OptionalType *self = (UtAsn1OptionalType *)object;
  return self->type;
}

bool ut_object_is_asn1_optional_type(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
