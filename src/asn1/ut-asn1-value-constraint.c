#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *value;
} UtAsn1ValueConstraint;

static char *ut_asn1_value_constraint_to_string(UtObject *object) {
  UtAsn1ValueConstraint *self = (UtAsn1ValueConstraint *)object;
  ut_cstring_ref value_string = ut_object_to_string(self->value);
  return ut_cstring_new_printf("<UtAsn1ValueConstraint>(%s)", value_string);
}

static void ut_asn1_value_constraint_cleanup(UtObject *object) {
  UtAsn1ValueConstraint *self = (UtAsn1ValueConstraint *)object;
  ut_object_unref(self->value);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1ValueConstraint",
    .to_string = ut_asn1_value_constraint_to_string,
    .cleanup = ut_asn1_value_constraint_cleanup};

UtObject *ut_asn1_value_constraint_new(UtObject *value) {
  UtObject *object =
      ut_object_new(sizeof(UtAsn1ValueConstraint), &object_interface);
  UtAsn1ValueConstraint *self = (UtAsn1ValueConstraint *)object;

  self->value = ut_object_ref(value);

  return object;
}

UtObject *ut_asn1_value_constraint_get_value(UtObject *object) {
  assert(ut_object_is_asn1_value_constraint(object));
  UtAsn1ValueConstraint *self = (UtAsn1ValueConstraint *)object;
  return self->value;
}

bool ut_object_is_asn1_value_constraint(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
