#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  int64_t lower;
  int64_t upper;
} UtAsn1IntegerRangeConstraint;

static char *ut_asn1_integer_range_constraint_to_string(UtObject *object) {
  UtAsn1IntegerRangeConstraint *self = (UtAsn1IntegerRangeConstraint *)object;
  return ut_cstring_new_printf("<UtAsn1IntegerRangeConstraint>(%li, %li)",
                               self->lower, self->upper);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1IntegerRangeConstraint",
    .to_string = ut_asn1_integer_range_constraint_to_string};

UtObject *ut_asn1_integer_range_constraint_new(int64_t lower, int64_t upper) {
  UtObject *object =
      ut_object_new(sizeof(UtAsn1IntegerRangeConstraint), &object_interface);
  UtAsn1IntegerRangeConstraint *self = (UtAsn1IntegerRangeConstraint *)object;

  self->lower = lower;
  self->upper = upper;

  return object;
}

int64_t ut_asn1_integer_range_constraint_get_lower(UtObject *object) {
  assert(ut_object_is_asn1_integer_range_constraint(object));
  UtAsn1IntegerRangeConstraint *self = (UtAsn1IntegerRangeConstraint *)object;
  return self->lower;
}

int64_t ut_asn1_integer_range_constraint_get_upper(UtObject *object) {
  assert(ut_object_is_asn1_integer_range_constraint(object));
  UtAsn1IntegerRangeConstraint *self = (UtAsn1IntegerRangeConstraint *)object;
  return self->upper;
}

bool ut_object_is_asn1_integer_range_constraint(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
