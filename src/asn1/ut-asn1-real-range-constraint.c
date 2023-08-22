#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  double lower;
  double upper;
} UtAsn1RealRangeConstraint;

static char *ut_asn1_real_range_constraint_to_string(UtObject *object) {
  UtAsn1RealRangeConstraint *self = (UtAsn1RealRangeConstraint *)object;
  return ut_cstring_new_printf("<UtAsn1RealRangeConstraint>(%f, %f)",
                               self->lower, self->upper);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1RealRangeConstraint",
    .to_string = ut_asn1_real_range_constraint_to_string};

UtObject *ut_asn1_real_range_constraint_new(double lower, double upper) {
  UtObject *object =
      ut_object_new(sizeof(UtAsn1RealRangeConstraint), &object_interface);
  UtAsn1RealRangeConstraint *self = (UtAsn1RealRangeConstraint *)object;

  self->lower = lower;
  self->upper = upper;

  return object;
}

double ut_asn1_real_range_constraint_get_lower(UtObject *object) {
  assert(ut_object_is_asn1_real_range_constraint(object));
  UtAsn1RealRangeConstraint *self = (UtAsn1RealRangeConstraint *)object;
  return self->lower;
}

double ut_asn1_real_range_constraint_get_upper(UtObject *object) {
  assert(ut_object_is_asn1_real_range_constraint(object));
  UtAsn1RealRangeConstraint *self = (UtAsn1RealRangeConstraint *)object;
  return self->upper;
}

bool ut_object_is_asn1_real_range_constraint(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
