#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *abstract;
  UtObject *transfer;
} UtAsn1EmbeddedIdentificationSyntaxes;

static char *
ut_asn1_embedded_identification_syntaxes_to_string(UtObject *object) {
  UtAsn1EmbeddedIdentificationSyntaxes *self =
      (UtAsn1EmbeddedIdentificationSyntaxes *)object;
  ut_cstring_ref abstract_string = ut_object_to_string(self->abstract);
  ut_cstring_ref transfer_string = ut_object_to_string(self->transfer);
  return ut_cstring_new_printf("<UtAsn1EmbeddedIdentificationSyntaxes>(%s, %s)",
                               abstract_string, transfer_string);
}

static void ut_asn1_embedded_identification_syntaxes_cleanup(UtObject *object) {
  UtAsn1EmbeddedIdentificationSyntaxes *self =
      (UtAsn1EmbeddedIdentificationSyntaxes *)object;
  ut_object_unref(self->abstract);
  ut_object_unref(self->transfer);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1EmbeddedIdentificationSyntaxes",
    .to_string = ut_asn1_embedded_identification_syntaxes_to_string,
    .cleanup = ut_asn1_embedded_identification_syntaxes_cleanup};

UtObject *ut_asn1_embedded_identification_syntaxes_new(UtObject *abstract,
                                                       UtObject *transfer) {
  UtObject *object = ut_object_new(sizeof(UtAsn1EmbeddedIdentificationSyntaxes),
                                   &object_interface);
  UtAsn1EmbeddedIdentificationSyntaxes *self =
      (UtAsn1EmbeddedIdentificationSyntaxes *)object;

  self->abstract = ut_object_ref(abstract);
  self->transfer = ut_object_ref(transfer);

  return object;
}

UtObject *
ut_asn1_embedded_identification_syntaxes_get_abstract(UtObject *object) {
  assert(ut_object_is_asn1_embedded_identification_syntaxes(object));
  UtAsn1EmbeddedIdentificationSyntaxes *self =
      (UtAsn1EmbeddedIdentificationSyntaxes *)object;
  return self->abstract;
}

UtObject *
ut_asn1_embedded_identification_syntaxes_get_transfer(UtObject *object) {
  assert(ut_object_is_asn1_embedded_identification_syntaxes(object));
  UtAsn1EmbeddedIdentificationSyntaxes *self =
      (UtAsn1EmbeddedIdentificationSyntaxes *)object;
  return self->transfer;
}

bool ut_object_is_asn1_embedded_identification_syntaxes(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
