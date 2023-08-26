#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  int64_t presentation_context_id;
  UtObject *transfer_syntax;
} UtAsn1EmbeddedIdentificationContextNegotiation;

static char *ut_asn1_embedded_identification_context_negotiation_to_string(
    UtObject *object) {
  UtAsn1EmbeddedIdentificationContextNegotiation *self =
      (UtAsn1EmbeddedIdentificationContextNegotiation *)object;
  ut_cstring_ref transfer_syntax_string =
      ut_object_to_string(self->transfer_syntax);
  return ut_cstring_new_printf(
      "<UtAsn1EmbeddedIdentificationContextNegotiation>(%li, %s)",
      self->presentation_context_id, transfer_syntax_string);
}

static void
ut_asn1_embedded_identification_context_negotiation_cleanup(UtObject *object) {
  UtAsn1EmbeddedIdentificationContextNegotiation *self =
      (UtAsn1EmbeddedIdentificationContextNegotiation *)object;
  ut_object_unref(self->transfer_syntax);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1EmbeddedIdentificationContextNegotiation",
    .to_string = ut_asn1_embedded_identification_context_negotiation_to_string,
    .cleanup = ut_asn1_embedded_identification_context_negotiation_cleanup};

UtObject *ut_asn1_embedded_identification_context_negotiation_new(
    int64_t presentation_context_id, UtObject *transfer_syntax) {
  UtObject *object =
      ut_object_new(sizeof(UtAsn1EmbeddedIdentificationContextNegotiation),
                    &object_interface);
  UtAsn1EmbeddedIdentificationContextNegotiation *self =
      (UtAsn1EmbeddedIdentificationContextNegotiation *)object;

  self->presentation_context_id = presentation_context_id;
  self->transfer_syntax = ut_object_ref(transfer_syntax);

  return object;
}

int64_t
ut_asn1_embedded_identification_context_negotiation_get_presentation_context_id(
    UtObject *object) {
  assert(ut_object_is_asn1_embedded_identification_context_negotiation(object));
  UtAsn1EmbeddedIdentificationContextNegotiation *self =
      (UtAsn1EmbeddedIdentificationContextNegotiation *)object;
  return self->presentation_context_id;
}

UtObject *
ut_asn1_embedded_identification_context_negotiation_get_transfer_syntax(
    UtObject *object) {
  assert(ut_object_is_asn1_embedded_identification_context_negotiation(object));
  UtAsn1EmbeddedIdentificationContextNegotiation *self =
      (UtAsn1EmbeddedIdentificationContextNegotiation *)object;
  return self->transfer_syntax;
}

bool ut_object_is_asn1_embedded_identification_context_negotiation(
    UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
