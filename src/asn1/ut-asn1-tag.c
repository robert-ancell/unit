#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;

  UtAsn1TagClass class;
  uint32_t number;
} UtAsn1Tag;

static bool tag_matches(UtAsn1Tag *self, UtAsn1TagClass class,
                        uint32_t number) {
  return self->class == class && self->number == number;
}

static char *ut_asn1_tag_to_string(UtObject *object) {
  UtAsn1Tag *self = (UtAsn1Tag *)object;
  return ut_cstring_new_printf("<UtAsn1Tag>(%d, %d)", self->class,
                               self->number);
}

static bool ut_asn1_tag_equal(UtObject *object, UtObject *other) {
  UtAsn1Tag *self = (UtAsn1Tag *)object;
  UtAsn1Tag *other_self = (UtAsn1Tag *)other;
  return ut_object_is_asn1_tag(other) &&
         tag_matches(self, other_self->class, other_self->number);
}

static UtObjectInterface object_interface = {.type_name = "UtAsn1Tag",
                                             .to_string = ut_asn1_tag_to_string,
                                             .equal = ut_asn1_tag_equal};

UtObject *ut_asn1_tag_new(UtAsn1TagClass class, uint32_t number) {
  UtObject *object = ut_object_new(sizeof(UtAsn1Tag), &object_interface);
  UtAsn1Tag *self = (UtAsn1Tag *)object;
  self->class = class;
  self->number = number;
  return object;
}

UtObject *ut_asn1_tag_new_universal(UtAsn1TagUniversal type) {
  return ut_asn1_tag_new(UT_ASN1_TAG_CLASS_UNIVERSAL, type);
}

UtAsn1TagClass ut_asn1_tag_get_class(UtObject *object) {
  assert(ut_object_is_asn1_tag(object));
  UtAsn1Tag *self = (UtAsn1Tag *)object;
  return self->class;
}

uint32_t ut_asn1_tag_get_number(UtObject *object) {
  assert(ut_object_is_asn1_tag(object));
  UtAsn1Tag *self = (UtAsn1Tag *)object;
  return self->number;
}

bool ut_asn1_tag_matches(UtObject *object, UtAsn1TagClass class,
                         uint32_t number) {
  assert(ut_object_is_asn1_tag(object));
  UtAsn1Tag *self = (UtAsn1Tag *)object;
  return tag_matches(self, class, number);
}

/// Returns [true] if [object] is a [UtAsn1Tag].
bool ut_object_is_asn1_tag(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
