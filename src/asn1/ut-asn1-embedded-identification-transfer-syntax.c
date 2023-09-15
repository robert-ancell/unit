#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *identifier;
} UtAsn1EmbeddedIdentificationTransferSyntax;

static char *
ut_asn1_embedded_identification_transfer_syntax_to_string(UtObject *object) {
  UtAsn1EmbeddedIdentificationTransferSyntax *self =
      (UtAsn1EmbeddedIdentificationTransferSyntax *)object;
  ut_cstring_ref identifier_string = ut_object_to_string(self->identifier);
  return ut_cstring_new_printf(
      "<UtAsn1EmbeddedIdentificationTransferSyntax>(%s)", identifier_string);
}

static void
ut_asn1_embedded_identification_transfer_syntax_cleanup(UtObject *object) {
  UtAsn1EmbeddedIdentificationTransferSyntax *self =
      (UtAsn1EmbeddedIdentificationTransferSyntax *)object;
  ut_object_unref(self->identifier);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1EmbeddedIdentificationTransferSyntax",
    .to_string = ut_asn1_embedded_identification_transfer_syntax_to_string,
    .cleanup = ut_asn1_embedded_identification_transfer_syntax_cleanup};

UtObject *
ut_asn1_embedded_identification_transfer_syntax_new(UtObject *identifier) {
  UtObject *object = ut_object_new(
      sizeof(UtAsn1EmbeddedIdentificationTransferSyntax), &object_interface);
  UtAsn1EmbeddedIdentificationTransferSyntax *self =
      (UtAsn1EmbeddedIdentificationTransferSyntax *)object;

  self->identifier = ut_object_ref(identifier);

  return object;
}

UtObject *ut_asn1_embedded_identification_transfer_syntax_get_identifier(
    UtObject *object) {
  assert(ut_object_is_asn1_embedded_identification_transfer_syntax(object));
  UtAsn1EmbeddedIdentificationTransferSyntax *self =
      (UtAsn1EmbeddedIdentificationTransferSyntax *)object;
  return self->identifier;
}

bool ut_object_is_asn1_embedded_identification_transfer_syntax(
    UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
