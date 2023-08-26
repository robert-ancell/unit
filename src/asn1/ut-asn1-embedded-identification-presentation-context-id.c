#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  int64_t value;
} UtAsn1EmbeddedIdentificationPresentationContextId;

static char *ut_asn1_embedded_identification_presentation_context_id_to_string(
    UtObject *object) {
  UtAsn1EmbeddedIdentificationPresentationContextId *self =
      (UtAsn1EmbeddedIdentificationPresentationContextId *)object;
  return ut_cstring_new_printf(
      "<UtAsn1EmbeddedIdentificationPresentationContextId>(%li)", self->value);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1EmbeddedIdentificationPresentationContextId",
    .to_string =
        ut_asn1_embedded_identification_presentation_context_id_to_string};

UtObject *
ut_asn1_embedded_identification_presentation_context_id_new(int64_t value) {
  UtObject *object =
      ut_object_new(sizeof(UtAsn1EmbeddedIdentificationPresentationContextId),
                    &object_interface);
  UtAsn1EmbeddedIdentificationPresentationContextId *self =
      (UtAsn1EmbeddedIdentificationPresentationContextId *)object;

  self->value = value;

  return object;
}

int64_t ut_asn1_embedded_identification_presentation_context_id_get_value(
    UtObject *object) {
  assert(ut_object_is_asn1_embedded_identification_presentation_context_id(
      object));
  UtAsn1EmbeddedIdentificationPresentationContextId *self =
      (UtAsn1EmbeddedIdentificationPresentationContextId *)object;
  return self->value;
}

bool ut_object_is_asn1_embedded_identification_presentation_context_id(
    UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
