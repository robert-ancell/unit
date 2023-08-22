#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *type;
  UtObject *default_value;
} UtAsn1DefaultType;

static UtObject *ut_asn1_default_type_get_tags(UtObject *object) {
  UtAsn1DefaultType *self = (UtAsn1DefaultType *)object;
  return ut_asn1_type_get_tags(self->type);
}

static bool ut_asn1_default_type_matches_tag(UtObject *object, UtObject *tag) {
  UtAsn1DefaultType *self = (UtAsn1DefaultType *)object;
  return ut_asn1_type_matches_tag(self->type, tag);
}

static UtObject *ut_asn1_default_type_get_base_type(UtObject *object) {
  UtAsn1DefaultType *self = (UtAsn1DefaultType *)object;
  return self->type;
}

static UtAsn1TypeInterface asn1_type_interface = {
    .get_tags = ut_asn1_default_type_get_tags,
    .matches_tag = ut_asn1_default_type_matches_tag,
    .get_base_type = ut_asn1_default_type_get_base_type};

static char *ut_asn1_default_type_to_string(UtObject *object) {
  UtAsn1DefaultType *self = (UtAsn1DefaultType *)object;
  ut_cstring_ref type_string = ut_object_to_string(self->type);
  ut_cstring_ref default_value_string =
      ut_object_to_string(self->default_value);
  return ut_cstring_new_printf("<UtAsn1DefaultType>(%s, %s)", type_string,
                               default_value_string);
}

static bool ut_asn1_default_type_equal(UtObject *object, UtObject *other) {
  UtAsn1DefaultType *self = (UtAsn1DefaultType *)object;
  UtAsn1DefaultType *other_self = (UtAsn1DefaultType *)other;
  return ut_object_equal(self->type, ut_object_is_asn1_default_type(other)
                                         ? other_self->type
                                         : other);
}

static void ut_asn1_default_type_cleanup(UtObject *object) {
  UtAsn1DefaultType *self = (UtAsn1DefaultType *)object;
  ut_object_unref(self->type);
  ut_object_unref(self->default_value);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1DefaultType",
    .to_string = ut_asn1_default_type_to_string,
    .equal = ut_asn1_default_type_equal,
    .cleanup = ut_asn1_default_type_cleanup,
    .interfaces = {{&ut_asn1_type_id, &asn1_type_interface}, {NULL, NULL}}};

UtObject *ut_asn1_default_type_new(UtObject *type, UtObject *default_value) {
  UtObject *object =
      ut_object_new(sizeof(UtAsn1DefaultType), &object_interface);
  UtAsn1DefaultType *self = (UtAsn1DefaultType *)object;

  self->type = ut_object_ref(type);
  self->default_value = ut_object_ref(default_value);

  return object;
}

UtObject *ut_asn1_default_type_new_take(UtObject *type,
                                        UtObject *default_value) {
  UtObject *object = ut_asn1_default_type_new(type, default_value);
  ut_object_unref(type);
  ut_object_unref(default_value);
  return object;
}

UtObject *ut_asn1_default_type_get_type(UtObject *object) {
  assert(ut_object_is_asn1_default_type(object));
  UtAsn1DefaultType *self = (UtAsn1DefaultType *)object;
  return self->type;
}

UtObject *ut_asn1_default_type_get_default_value(UtObject *object) {
  assert(ut_object_is_asn1_default_type(object));
  UtAsn1DefaultType *self = (UtAsn1DefaultType *)object;
  return self->default_value;
}

bool ut_object_is_asn1_default_type(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
