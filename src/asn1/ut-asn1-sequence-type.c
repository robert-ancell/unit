#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *components;
  bool extensible;
} UtAsn1SequenceType;

static char *ut_asn1_sequence_type_to_string(UtObject *object) {
  UtAsn1SequenceType *self = (UtAsn1SequenceType *)object;
  ut_cstring_ref components_string = ut_object_to_string(self->components);
  return ut_cstring_new_printf("<UtAsn1SequenceType>(%s)", components_string);
}

static void ut_asn1_sequence_type_cleanup(UtObject *object) {
  UtAsn1SequenceType *self = (UtAsn1SequenceType *)object;
  ut_object_unref(self->components);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1SequenceType",
    .to_string = ut_asn1_sequence_type_to_string,
    .cleanup = ut_asn1_sequence_type_cleanup,
    .interfaces = {{NULL, NULL}}};

UtObject *ut_asn1_sequence_type_new(UtObject *components, bool extensible) {
  UtObject *object =
      ut_object_new(sizeof(UtAsn1SequenceType), &object_interface);
  UtAsn1SequenceType *self = (UtAsn1SequenceType *)object;

  self->components = ut_object_ref(components);
  self->extensible = extensible;

  return object;
}

UtObject *ut_asn1_sequence_type_get_components(UtObject *object) {
  assert(ut_object_is_asn1_sequence_type(object));
  UtAsn1SequenceType *self = (UtAsn1SequenceType *)object;
  return self->components;
}

bool ut_asn1_sequence_type_get_extensible(UtObject *object) {
  assert(ut_object_is_asn1_sequence_type(object));
  UtAsn1SequenceType *self = (UtAsn1SequenceType *)object;
  return self->extensible;
}

bool ut_object_is_asn1_sequence_type(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
