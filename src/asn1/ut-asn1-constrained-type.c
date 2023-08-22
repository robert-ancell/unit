#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *type;
  UtObject *constraint;
} UtAsn1ConstrainedType;

static UtObject *ut_asn1_constrained_type_get_tags(UtObject *object) {
  UtAsn1ConstrainedType *self = (UtAsn1ConstrainedType *)object;
  return ut_asn1_type_get_tags(self->type);
}

static UtObject *ut_asn1_constrained_type_get_base_type(UtObject *object) {
  UtAsn1ConstrainedType *self = (UtAsn1ConstrainedType *)object;
  return self->type;
}

static UtAsn1TypeInterface asn1_type_interface = {
    .get_tags = ut_asn1_constrained_type_get_tags,
    .get_base_type = ut_asn1_constrained_type_get_base_type};

static char *ut_asn1_constrained_type_to_string(UtObject *object) {
  UtAsn1ConstrainedType *self = (UtAsn1ConstrainedType *)object;
  ut_cstring_ref type_string = ut_object_to_string(self->type);
  ut_cstring_ref constraint_string = ut_object_to_string(self->constraint);
  return ut_cstring_new_printf("<UtAsn1ConstrainedType>(%s, %s)", type_string,
                               constraint_string);
}

static void ut_asn1_constrained_type_cleanup(UtObject *object) {
  UtAsn1ConstrainedType *self = (UtAsn1ConstrainedType *)object;
  ut_object_unref(self->type);
  ut_object_unref(self->constraint);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1ConstrainedType",
    .to_string = ut_asn1_constrained_type_to_string,
    .cleanup = ut_asn1_constrained_type_cleanup,
    .interfaces = {{&ut_asn1_type_id, &asn1_type_interface}, {NULL, NULL}}};

UtObject *ut_asn1_constrained_type_new(UtObject *type, UtObject *constraint) {
  UtObject *object =
      ut_object_new(sizeof(UtAsn1ConstrainedType), &object_interface);
  UtAsn1ConstrainedType *self = (UtAsn1ConstrainedType *)object;

  self->type = ut_object_ref(type);
  self->constraint = ut_object_ref(constraint);

  return object;
}

UtObject *ut_asn1_constrained_type_new_take(UtObject *type,
                                            UtObject *constraint) {
  UtObject *object = ut_asn1_constrained_type_new(type, constraint);
  ut_object_unref(type);
  ut_object_unref(constraint);
  return object;
}

UtObject *ut_asn1_constrained_type_get_type(UtObject *object) {
  assert(ut_object_is_asn1_constrained_type(object));
  UtAsn1ConstrainedType *self = (UtAsn1ConstrainedType *)object;
  return self->type;
}

UtObject *ut_asn1_constrained_type_get_constraint(UtObject *object) {
  assert(ut_object_is_asn1_constrained_type(object));
  UtAsn1ConstrainedType *self = (UtAsn1ConstrainedType *)object;
  return self->constraint;
}

bool ut_object_is_asn1_constrained_type(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
