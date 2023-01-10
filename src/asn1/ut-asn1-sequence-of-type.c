#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *type;
} UtAsn1SequenceOfType;

static char *ut_asn1_sequence_of_type_to_string(UtObject *object) {
  UtAsn1SequenceOfType *self = (UtAsn1SequenceOfType *)object;
  ut_cstring_ref type_string = ut_object_to_string(self->type);
  return ut_cstring_new_printf("<UtAsn1SequenceOfType>(%s)", type_string);
}

static void ut_asn1_sequence_of_type_cleanup(UtObject *object) {
  UtAsn1SequenceOfType *self = (UtAsn1SequenceOfType *)object;
  ut_object_unref(self->type);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1SequenceOfType",
    .to_string = ut_asn1_sequence_of_type_to_string,
    .cleanup = ut_asn1_sequence_of_type_cleanup,
    .interfaces = {{NULL, NULL}}};

UtObject *ut_asn1_sequence_of_type_new(UtObject *type) {
  UtObject *object =
      ut_object_new(sizeof(UtAsn1SequenceOfType), &object_interface);
  UtAsn1SequenceOfType *self = (UtAsn1SequenceOfType *)object;

  self->type = ut_object_ref(type);

  return object;
}

UtObject *ut_asn1_sequence_of_type_get_type(UtObject *object) {
  assert(ut_object_is_asn1_sequence_of_type(object));
  UtAsn1SequenceOfType *self = (UtAsn1SequenceOfType *)object;
  return self->type;
}

bool ut_object_is_asn1_sequence_of_type(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
