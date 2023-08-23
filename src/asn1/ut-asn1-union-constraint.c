#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *constraints;
} UtAsn1UnionConstraint;

static char *ut_asn1_union_constraint_to_string(UtObject *object) {
  UtAsn1UnionConstraint *self = (UtAsn1UnionConstraint *)object;
  ut_cstring_ref constraints_string = ut_object_to_string(self->constraints);
  return ut_cstring_new_printf("<UtAsn1UnionConstraint>(%s)",
                               constraints_string);
}

static void ut_asn1_union_constraint_cleanup(UtObject *object) {
  UtAsn1UnionConstraint *self = (UtAsn1UnionConstraint *)object;
  ut_object_unref(self->constraints);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1UnionConstraint",
    .to_string = ut_asn1_union_constraint_to_string,
    .cleanup = ut_asn1_union_constraint_cleanup};

UtObject *ut_asn1_union_constraint_new(UtObject *constraints) {
  UtObject *object =
      ut_object_new(sizeof(UtAsn1UnionConstraint), &object_interface);
  UtAsn1UnionConstraint *self = (UtAsn1UnionConstraint *)object;

  self->constraints = ut_object_ref(constraints);

  return object;
}

UtObject *ut_asn1_union_constraint_get_constraints(UtObject *object) {
  assert(ut_object_is_asn1_union_constraint(object));
  UtAsn1UnionConstraint *self = (UtAsn1UnionConstraint *)object;
  return self->constraints;
}

bool ut_object_is_asn1_union_constraint(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
