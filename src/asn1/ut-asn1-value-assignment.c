#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  char *reference;
  UtObject *type;
  UtObject *value;
} UtAsn1ValueAssignment;

static char *ut_asn1_value_assignment_to_string(UtObject *object) {
  UtAsn1ValueAssignment *self = (UtAsn1ValueAssignment *)object;
  ut_cstring_ref type_string = ut_object_to_string(self->type);
  ut_cstring_ref value_string = ut_object_to_string(self->value);
  return ut_cstring_new_printf("<UtAsn1ValueAssignment>(\"%s\", %s, %s)",
                               self->reference, type_string, value_string);
}

static void ut_asn1_value_assignment_cleanup(UtObject *object) {
  UtAsn1ValueAssignment *self = (UtAsn1ValueAssignment *)object;
  ut_object_unref(self->type);
  ut_object_unref(self->value);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1ValueAssignment",
    .to_string = ut_asn1_value_assignment_to_string,
    .cleanup = ut_asn1_value_assignment_cleanup,
    .interfaces = {{NULL, NULL}}};

UtObject *ut_asn1_value_assignment_new(const char *reference, UtObject *type,
                                       UtObject *value) {
  UtObject *object =
      ut_object_new(sizeof(UtAsn1ValueAssignment), &object_interface);
  UtAsn1ValueAssignment *self = (UtAsn1ValueAssignment *)object;

  self->reference = ut_cstring_new(reference);
  self->type = ut_object_ref(type);
  self->value = ut_object_ref(value);

  return object;
}

const char *ut_asn1_value_assignment_get_reference(UtObject *object) {
  assert(ut_object_is_asn1_value_assignment(object));
  UtAsn1ValueAssignment *self = (UtAsn1ValueAssignment *)object;
  return self->reference;
}

UtObject *ut_asn1_value_assignment_get_type(UtObject *object) {
  assert(ut_object_is_asn1_value_assignment(object));
  UtAsn1ValueAssignment *self = (UtAsn1ValueAssignment *)object;
  return self->type;
}

UtObject *ut_asn1_value_assignment_get_value(UtObject *object) {
  assert(ut_object_is_asn1_value_assignment(object));
  UtAsn1ValueAssignment *self = (UtAsn1ValueAssignment *)object;
  return self->value;
}

bool ut_object_is_asn1_value_assignment(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
