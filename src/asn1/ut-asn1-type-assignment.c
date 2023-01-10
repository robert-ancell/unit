#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  char *reference;
  UtObject *type;
} UtAsn1TypeAssignment;

static char *ut_asn1_type_assignment_to_string(UtObject *object) {
  UtAsn1TypeAssignment *self = (UtAsn1TypeAssignment *)object;
  ut_cstring_ref type_string = ut_object_to_string(self->type);
  return ut_cstring_new_printf("<UtAsn1TypeAssignment>(\"%s\", %s)",
                               self->reference, type_string);
}

static void ut_asn1_type_assignment_cleanup(UtObject *object) {
  UtAsn1TypeAssignment *self = (UtAsn1TypeAssignment *)object;
  ut_object_unref(self->type);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1TypeAssignment",
    .to_string = ut_asn1_type_assignment_to_string,
    .cleanup = ut_asn1_type_assignment_cleanup,
    .interfaces = {{NULL, NULL}}};

UtObject *ut_asn1_type_assignment_new(const char *reference, UtObject *type) {
  UtObject *object =
      ut_object_new(sizeof(UtAsn1TypeAssignment), &object_interface);
  UtAsn1TypeAssignment *self = (UtAsn1TypeAssignment *)object;

  self->reference = ut_cstring_new(reference);
  self->type = ut_object_ref(type);

  return object;
}

const char *ut_asn1_type_assignment_get_reference(UtObject *object) {
  assert(ut_object_is_asn1_type_assignment(object));
  UtAsn1TypeAssignment *self = (UtAsn1TypeAssignment *)object;
  return self->reference;
}

UtObject *ut_asn1_type_assignment_get_type(UtObject *object) {
  assert(ut_object_is_asn1_type_assignment(object));
  UtAsn1TypeAssignment *self = (UtAsn1TypeAssignment *)object;
  return self->type;
}

bool ut_object_is_asn1_type_assignment(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
