#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
} UtX509CertificateEncoder;

static char *ut_x509_certificate_encoder_to_string(UtObject *object) {
  // UtX509CertificateEncoder *self = (UtX509CertificateEncoder *)object;
  return ut_cstring_new("<UtX509CertificateEncoder>()");
}

static void ut_x509_certificate_encoder_cleanup(UtObject *object) {
  // UtX509CertificateEncoder *self = (UtX509CertificateEncoder *)object;
}

static UtObjectInterface object_interface = {
    .type_name = "UtX509CertificateEncoder",
    .to_string = ut_x509_certificate_encoder_to_string,
    .cleanup = ut_x509_certificate_encoder_cleanup};

UtObject *ut_x509_certificate_encoder_new() {
  return ut_object_new(sizeof(UtX509CertificateEncoder), &object_interface);
}

bool ut_object_is_x509_certificate_encoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
