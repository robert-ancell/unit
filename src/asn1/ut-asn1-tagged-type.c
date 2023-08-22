#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtAsn1TagClass class;
  unsigned int number;
  bool is_explicit;
  UtObject *type;
} UtAsn1TaggedType;

static UtObject *ut_asn1_tagged_type_get_tags(UtObject *object) {
  UtAsn1TaggedType *self = (UtAsn1TaggedType *)object;
  return ut_list_new_from_elements_take(
      ut_asn1_tag_new(self->class, self->number), NULL);
}

static bool ut_asn1_tagged_type_matches_tag(UtObject *object, UtObject *tag) {
  UtAsn1TaggedType *self = (UtAsn1TaggedType *)object;
  return ut_asn1_tag_matches(tag, self->class, self->number);
}

static UtObject *ut_asn1_tagged_type_get_base_type(UtObject *object) {
  UtAsn1TaggedType *self = (UtAsn1TaggedType *)object;
  return self->type;
}

static UtAsn1TypeInterface asn1_type_interface = {
    .get_tags = ut_asn1_tagged_type_get_tags,
    .matches_tag = ut_asn1_tagged_type_matches_tag,
    .get_base_type = ut_asn1_tagged_type_get_base_type};

static char *ut_asn1_tagged_type_to_string(UtObject *object) {
  UtAsn1TaggedType *self = (UtAsn1TaggedType *)object;
  ut_cstring_ref type_string = ut_object_to_string(self->type);
  return ut_cstring_new_printf("<UtAsn1TaggedType>(%d, %u, %s)", self->class,
                               self->number, type_string);
}

static void ut_asn1_tagged_type_cleanup(UtObject *object) {
  UtAsn1TaggedType *self = (UtAsn1TaggedType *)object;
  ut_object_unref(self->type);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1TaggedType",
    .to_string = ut_asn1_tagged_type_to_string,
    .cleanup = ut_asn1_tagged_type_cleanup,
    .interfaces = {{&ut_asn1_type_id, &asn1_type_interface}, {NULL, NULL}}};

UtObject *ut_asn1_tagged_type_new(UtAsn1TagClass class, unsigned int number,
                                  bool is_explicit, UtObject *type) {
  UtObject *object = ut_object_new(sizeof(UtAsn1TaggedType), &object_interface);
  UtAsn1TaggedType *self = (UtAsn1TaggedType *)object;

  self->class = class;
  self->number = number;
  self->is_explicit = is_explicit;
  self->type = ut_object_ref(type);

  return object;
}

UtObject *ut_asn1_tagged_type_new_take(UtAsn1TagClass class,
                                       unsigned int number, bool is_explicit,
                                       UtObject *type) {
  UtObject *object = ut_asn1_tagged_type_new(class, number, is_explicit, type);
  ut_object_unref(type);
  return object;
}

UtAsn1TagClass ut_asn1_tagged_type_get_class(UtObject *object) {
  assert(ut_object_is_asn1_tagged_type(object));
  UtAsn1TaggedType *self = (UtAsn1TaggedType *)object;
  return self->class;
}

unsigned int ut_asn1_tagged_type_get_number(UtObject *object) {
  assert(ut_object_is_asn1_tagged_type(object));
  UtAsn1TaggedType *self = (UtAsn1TaggedType *)object;
  return self->number;
}

bool ut_asn1_tagged_type_get_is_explicit(UtObject *object) {
  assert(ut_object_is_asn1_tagged_type(object));
  UtAsn1TaggedType *self = (UtAsn1TaggedType *)object;
  return self->is_explicit;
}

UtObject *ut_asn1_tagged_type_get_type(UtObject *object) {
  assert(ut_object_is_asn1_tagged_type(object));
  UtAsn1TaggedType *self = (UtAsn1TaggedType *)object;
  return self->type;
}

bool ut_object_is_asn1_tagged_type(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
