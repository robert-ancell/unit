#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *identification;
  char *data_value_descriptor;
  UtObject *data_value;
} UtAsn1EmbeddedValue;

static char *ut_asn1_embedded_value_to_string(UtObject *object) {
  UtAsn1EmbeddedValue *self = (UtAsn1EmbeddedValue *)object;
  ut_cstring_ref identification_string =
      ut_object_to_string(self->identification);
  ut_cstring_ref data_value_string = ut_object_to_string(self->data_value);
  return ut_cstring_new_printf("<UtAsn1EmbeddedValue>(%s, \"%s\", %s)",
                               identification_string,
                               self->data_value_descriptor, data_value_string);
}

static void ut_asn1_embedded_value_cleanup(UtObject *object) {
  UtAsn1EmbeddedValue *self = (UtAsn1EmbeddedValue *)object;
  ut_object_unref(self->identification);
  free(self->data_value_descriptor);
  ut_object_unref(self->data_value);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1EmbeddedValue",
    .to_string = ut_asn1_embedded_value_to_string,
    .cleanup = ut_asn1_embedded_value_cleanup};

UtObject *ut_asn1_embedded_value_new(UtObject *identification,
                                     const char *data_value_descriptor,
                                     UtObject *data_value) {
  UtObject *object =
      ut_object_new(sizeof(UtAsn1EmbeddedValue), &object_interface);
  UtAsn1EmbeddedValue *self = (UtAsn1EmbeddedValue *)object;

  self->identification = ut_object_ref(identification);
  self->data_value_descriptor = data_value_descriptor != NULL
                                    ? ut_cstring_new(data_value_descriptor)
                                    : NULL;
  self->data_value = ut_object_ref(data_value);

  return object;
}

UtObject *ut_asn1_embedded_value_new_take(UtObject *identification,
                                          const char *data_value_descriptor,
                                          UtObject *data_value) {
  UtObject *object = ut_asn1_embedded_value_new(
      identification, data_value_descriptor, data_value);
  ut_object_unref(identification);
  ut_object_unref(data_value);
  return object;
}

UtObject *ut_asn1_embedded_value_get_identification(UtObject *object) {
  assert(ut_object_is_asn1_embedded_value(object));
  UtAsn1EmbeddedValue *self = (UtAsn1EmbeddedValue *)object;
  return self->identification;
}

const char *ut_asn1_embedded_value_get_data_value_descriptor(UtObject *object) {
  assert(ut_object_is_asn1_embedded_value(object));
  UtAsn1EmbeddedValue *self = (UtAsn1EmbeddedValue *)object;
  return self->data_value_descriptor;
}

UtObject *ut_asn1_embedded_value_get_data_value(UtObject *object) {
  assert(ut_object_is_asn1_embedded_value(object));
  UtAsn1EmbeddedValue *self = (UtAsn1EmbeddedValue *)object;
  return self->data_value;
}

bool ut_object_is_asn1_embedded_value(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
