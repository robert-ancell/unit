#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *constraint;
} UtAsn1SizeConstraint;

static char *ut_asn1_size_constraint_to_string(UtObject *object) {
  UtAsn1SizeConstraint *self = (UtAsn1SizeConstraint *)object;
  ut_cstring_ref constraint_string = ut_object_to_string(self->constraint);
  return ut_cstring_new_printf("<UtAsn1SizeConstraint>(%s)", constraint_string);
}

static void ut_asn1_size_constraint_cleanup(UtObject *object) {
  UtAsn1SizeConstraint *self = (UtAsn1SizeConstraint *)object;
  ut_object_unref(self->constraint);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1SizeConstraint",
    .to_string = ut_asn1_size_constraint_to_string,
    .cleanup = ut_asn1_size_constraint_cleanup};

UtObject *ut_asn1_size_constraint_new(UtObject *constraint) {
  UtObject *object =
      ut_object_new(sizeof(UtAsn1SizeConstraint), &object_interface);
  UtAsn1SizeConstraint *self = (UtAsn1SizeConstraint *)object;

  self->constraint = ut_object_ref(constraint);

  return object;
}

UtObject *ut_asn1_size_constraint_get_constraint(UtObject *object) {
  assert(ut_object_is_asn1_size_constraint(object));
  UtAsn1SizeConstraint *self = (UtAsn1SizeConstraint *)object;
  return self->constraint;
}

bool ut_object_is_asn1_size_constraint(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
