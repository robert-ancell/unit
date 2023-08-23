#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
} UtX509CertificateDecoder;

static char *ut_x509_certificate_decoder_to_string(UtObject *object) {
  // UtX509CertificateDecoder *self = (UtX509CertificateDecoder *)object;
  return ut_cstring_new("<UtX509CertificateDecoder>()");
}

static void ut_x509_certificate_decoder_cleanup(UtObject *object) {
  // UtX509CertificateDecoder *self = (UtX509CertificateDecoder *)object;
}

static UtObjectInterface object_interface = {
    .type_name = "UtX509CertificateDecoder",
    .to_string = ut_x509_certificate_decoder_to_string,
    .cleanup = ut_x509_certificate_decoder_cleanup};

UtObject *ut_x509_certificate_decoder_new() {
  return ut_object_new(sizeof(UtX509CertificateDecoder), &object_interface);
}

bool ut_object_is_x509_certificate_decoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
