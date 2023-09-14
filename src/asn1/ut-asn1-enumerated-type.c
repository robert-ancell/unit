#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *values_by_name;
  UtObject *names_by_value;
  bool extensible;
} UtAsn1EnumeratedType;

static UtObject *ut_asn1_enumerated_type_get_tags(UtObject *object) {
  return ut_list_new_from_elements_take(
      ut_asn1_tag_new_universal(UT_ASN1_TAG_UNIVERSAL_ENUMERATED), NULL);
}

static bool ut_asn1_enumerated_type_matches_tag(UtObject *object,
                                                UtObject *tag) {
  return ut_asn1_tag_matches(tag, UT_ASN1_TAG_CLASS_UNIVERSAL,
                             UT_ASN1_TAG_UNIVERSAL_ENUMERATED);
}

static UtAsn1TypeInterface asn1_type_interface = {
    .get_tags = ut_asn1_enumerated_type_get_tags,
    .matches_tag = ut_asn1_enumerated_type_matches_tag};

static char *ut_asn1_enumerated_type_to_string(UtObject *object) {
  UtAsn1EnumeratedType *self = (UtAsn1EnumeratedType *)object;
  ut_cstring_ref values_by_name_string =
      ut_object_to_string(self->values_by_name);
  return ut_cstring_new_printf("<UtAsn1EnumeratedType>(%s)",
                               values_by_name_string);
}

static void ut_asn1_enumerated_type_cleanup(UtObject *object) {
  UtAsn1EnumeratedType *self = (UtAsn1EnumeratedType *)object;
  ut_object_unref(self->values_by_name);
  ut_object_unref(self->names_by_value);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1EnumeratedType",
    .to_string = ut_asn1_enumerated_type_to_string,
    .cleanup = ut_asn1_enumerated_type_cleanup,
    .interfaces = {{&ut_asn1_type_id, &asn1_type_interface}, {NULL, NULL}}};

UtObject *ut_asn1_enumerated_type_new(UtObject *values_by_name,
                                      bool extensible) {
  UtObject *object =
      ut_object_new(sizeof(UtAsn1EnumeratedType), &object_interface);
  UtAsn1EnumeratedType *self = (UtAsn1EnumeratedType *)object;

  self->values_by_name = ut_object_ref(values_by_name);
  self->names_by_value = ut_map_new();
  UtObjectRef items = ut_map_get_items(values_by_name);
  size_t items_length = ut_list_get_length(items);
  for (size_t i = 0; i < items_length; i++) {
    UtObject *item = ut_object_list_get_element(items, i);
    ut_map_insert(self->names_by_value, ut_map_item_get_value(item),
                  ut_map_item_get_key(item));
  }
  self->extensible = extensible;

  return object;
}

int64_t ut_asn1_enumerated_type_lookup_value(UtObject *object,
                                             const char *name) {
  assert(ut_object_is_asn1_enumerated_type(object));
  UtAsn1EnumeratedType *self = (UtAsn1EnumeratedType *)object;
  UtObject *value = ut_map_lookup_string(self->values_by_name, name);
  if (value == NULL) {
    return -1;
  } else {
    return ut_uint64_get_value(value);
  }
}

const char *ut_asn1_enumerated_type_lookup_name(UtObject *object,
                                                int64_t number) {
  assert(ut_object_is_asn1_enumerated_type(object));
  UtAsn1EnumeratedType *self = (UtAsn1EnumeratedType *)object;
  if (number < 0) {
    return NULL;
  }
  UtObjectRef number_object = ut_uint64_new(number);
  UtObject *name = ut_map_lookup(self->names_by_value, number_object);
  return name != NULL ? ut_string_get_text(name) : NULL;
}

bool ut_asn1_enumerated_type_get_extensible(UtObject *object) {
  assert(ut_object_is_asn1_enumerated_type(object));
  UtAsn1EnumeratedType *self = (UtAsn1EnumeratedType *)object;
  return self->extensible;
}

bool ut_object_is_asn1_enumerated_type(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
