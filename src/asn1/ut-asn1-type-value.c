#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *type;
  UtObject *value;
} UtAsn1TypeValue;

static char *ut_asn1_type_value_to_string(UtObject *object) {
  UtAsn1TypeValue *self = (UtAsn1TypeValue *)object;
  ut_cstring_ref type_string = ut_object_to_string(self->type);
  ut_cstring_ref value_string = ut_object_to_string(self->value);
  return ut_cstring_new_printf("<UtAsn1TypeValue>(%s, %s)", type_string,
                               value_string);
}

static void ut_asn1_type_value_cleanup(UtObject *object) {
  UtAsn1TypeValue *self = (UtAsn1TypeValue *)object;
  ut_object_unref(self->type);
  ut_object_unref(self->value);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1TypeValue",
    .to_string = ut_asn1_type_value_to_string,
    .cleanup = ut_asn1_type_value_cleanup};

UtObject *ut_asn1_type_value_new(UtObject *type, UtObject *value) {
  UtObject *object = ut_object_new(sizeof(UtAsn1TypeValue), &object_interface);
  UtAsn1TypeValue *self = (UtAsn1TypeValue *)object;

  self->type = ut_object_ref(type);
  self->value = ut_object_ref(value);

  return object;
}

UtObject *ut_asn1_type_value_get_type(UtObject *object) {
  assert(ut_object_is_asn1_type_value(object));
  UtAsn1TypeValue *self = (UtAsn1TypeValue *)object;
  return self->type;
}

UtObject *ut_asn1_type_value_get_value(UtObject *object) {
  assert(ut_object_is_asn1_type_value(object));
  UtAsn1TypeValue *self = (UtAsn1TypeValue *)object;
  return self->value;
}

bool ut_object_is_asn1_type_value(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
