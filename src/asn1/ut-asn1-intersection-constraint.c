#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *constraints;
} UtAsn1IntersectionConstraint;

static char *ut_asn1_intersection_constraint_to_string(UtObject *object) {
  UtAsn1IntersectionConstraint *self = (UtAsn1IntersectionConstraint *)object;
  ut_cstring_ref constraints_string = ut_object_to_string(self->constraints);
  return ut_cstring_new_printf("<UtAsn1IntersectionConstraint>(%s)",
                               constraints_string);
}

static void ut_asn1_intersection_constraint_cleanup(UtObject *object) {
  UtAsn1IntersectionConstraint *self = (UtAsn1IntersectionConstraint *)object;
  ut_object_unref(self->constraints);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1IntersectionConstraint",
    .to_string = ut_asn1_intersection_constraint_to_string,
    .cleanup = ut_asn1_intersection_constraint_cleanup};

UtObject *ut_asn1_intersection_constraint_new(UtObject *constraints) {
  UtObject *object =
      ut_object_new(sizeof(UtAsn1IntersectionConstraint), &object_interface);
  UtAsn1IntersectionConstraint *self = (UtAsn1IntersectionConstraint *)object;

  self->constraints = ut_object_ref(constraints);

  return object;
}

UtObject *ut_asn1_intersection_constraint_get_constraints(UtObject *object) {
  assert(ut_object_is_asn1_intersection_constraint(object));
  UtAsn1IntersectionConstraint *self = (UtAsn1IntersectionConstraint *)object;
  return self->constraints;
}

bool ut_object_is_asn1_intersection_constraint(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
