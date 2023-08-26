#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  char *identifier;
  UtObject *value;
} UtAsn1ChoiceValue;

static char *ut_asn1_choice_value_to_string(UtObject *object) {
  UtAsn1ChoiceValue *self = (UtAsn1ChoiceValue *)object;
  ut_cstring_ref value_string = ut_object_to_string(self->value);
  return ut_cstring_new_printf("<UtAsn1ChoiceValue>(\"%s\", %s)",
                               self->identifier, value_string);
}

static void ut_asn1_choice_value_cleanup(UtObject *object) {
  UtAsn1ChoiceValue *self = (UtAsn1ChoiceValue *)object;
  free(self->identifier);
  ut_object_unref(self->value);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1ChoiceValue",
    .to_string = ut_asn1_choice_value_to_string,
    .cleanup = ut_asn1_choice_value_cleanup};

UtObject *ut_asn1_choice_value_new(const char *identifier, UtObject *value) {
  UtObject *object =
      ut_object_new(sizeof(UtAsn1ChoiceValue), &object_interface);
  UtAsn1ChoiceValue *self = (UtAsn1ChoiceValue *)object;

  self->identifier = ut_cstring_new(identifier);
  self->value = ut_object_ref(value);

  return object;
}

UtObject *ut_asn1_choice_value_new_take(const char *identifier,
                                        UtObject *value) {
  UtObject *object = ut_asn1_choice_value_new(identifier, value);
  ut_object_unref(value);
  return object;
}

const char *ut_asn1_choice_value_get_identifier(UtObject *object) {
  assert(ut_object_is_asn1_choice_value(object));
  UtAsn1ChoiceValue *self = (UtAsn1ChoiceValue *)object;
  return self->identifier;
}

UtObject *ut_asn1_choice_value_get_value(UtObject *object) {
  assert(ut_object_is_asn1_choice_value(object));
  UtAsn1ChoiceValue *self = (UtAsn1ChoiceValue *)object;
  return self->value;
}

bool ut_object_is_asn1_choice_value(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
