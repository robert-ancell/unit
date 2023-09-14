#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *components;
  bool extensible;
} UtAsn1SetType;

static UtObject *insert_tags(UtObject *tag_map, UtObject *identifier,
                             UtObject *type) {
  UtObjectRef tags = ut_asn1_type_get_tags(type);
  size_t tags_length = ut_list_get_length(tags);
  for (size_t i = 0; i < tags_length; i++) {
    UtObject *tag = ut_object_list_get_element(tags, i);

    UtObject *existing_identifier = ut_map_lookup(tag_map, tag);
    if (existing_identifier != NULL) {
      ut_cstring_ref description =
          ut_cstring_new_printf("Components %s and %s has the same tag",
                                ut_string_get_text(existing_identifier),
                                ut_string_get_text(identifier));
      return ut_asn1_error_new(description);
    }

    ut_map_insert(tag_map, tag, identifier);
  }

  return NULL;
}

static UtObject *check_components(UtObject *components) {
  // All components must have unique tags.
  UtObjectRef tag_map = ut_map_new();
  UtObjectRef identifiers = ut_map_get_keys(components);
  size_t identifiers_length = ut_list_get_length(identifiers);
  for (size_t i = 0; i < identifiers_length; i++) {
    UtObjectRef identifier = ut_list_get_element(identifiers, i);
    UtObject *type =
        ut_map_lookup_string(components, ut_string_get_text(identifier));

    UtObject *error = insert_tags(tag_map, identifier, type);
    if (error != NULL) {
      return error;
    }
  }

  return NULL;
}

static UtObject *ut_asn1_set_type_get_tags(UtObject *object) {
  return ut_list_new_from_elements_take(
      ut_asn1_tag_new_universal(UT_ASN1_TAG_UNIVERSAL_SET), NULL);
}

static bool ut_asn1_set_type_matches_tag(UtObject *object, UtObject *tag) {
  return ut_asn1_tag_matches(tag, UT_ASN1_TAG_CLASS_UNIVERSAL,
                             UT_ASN1_TAG_UNIVERSAL_SET);
}

static UtAsn1TypeInterface asn1_type_interface = {
    .get_tags = ut_asn1_set_type_get_tags,
    .matches_tag = ut_asn1_set_type_matches_tag};

static char *ut_asn1_set_type_to_string(UtObject *object) {
  UtAsn1SetType *self = (UtAsn1SetType *)object;
  ut_cstring_ref components_string = ut_object_to_string(self->components);
  return ut_cstring_new_printf("<UtAsn1SetType>(%s)", components_string);
}

static void ut_asn1_set_type_cleanup(UtObject *object) {
  UtAsn1SetType *self = (UtAsn1SetType *)object;
  ut_object_unref(self->components);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1SetType",
    .to_string = ut_asn1_set_type_to_string,
    .cleanup = ut_asn1_set_type_cleanup,
    .interfaces = {{&ut_asn1_type_id, &asn1_type_interface}, {NULL, NULL}}};

UtObject *ut_asn1_set_type_new(UtObject *components, bool extensible) {
  UtObject *error = check_components(components);
  if (error != NULL) {
    return error;
  }

  UtObject *object = ut_object_new(sizeof(UtAsn1SetType), &object_interface);
  UtAsn1SetType *self = (UtAsn1SetType *)object;

  self->components = ut_object_ref(components);
  self->extensible = extensible;

  return object;
}

UtObject *ut_asn1_set_type_new_take(UtObject *components, bool extensible) {
  UtObject *object = ut_asn1_set_type_new(components, extensible);
  ut_object_unref(components);
  return object;
}

UtObject *ut_asn1_set_type_get_components(UtObject *object) {
  assert(ut_object_is_asn1_set_type(object));
  UtAsn1SetType *self = (UtAsn1SetType *)object;
  return self->components;
}

bool ut_asn1_set_type_get_extensible(UtObject *object) {
  assert(ut_object_is_asn1_set_type(object));
  UtAsn1SetType *self = (UtAsn1SetType *)object;
  return self->extensible;
}

UtObject *ut_asn1_set_type_lookup_component(UtObject *object,
                                            const char *name) {
  assert(ut_object_is_asn1_set_type(object));
  UtAsn1SetType *self = (UtAsn1SetType *)object;
  return ut_map_lookup_string(self->components, name);
}

bool ut_object_is_asn1_set_type(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
