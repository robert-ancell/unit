#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
} UtX509Certificate;

static void ut_x509_certificate_cleanup(UtObject *object) {
  // UtX509Certificate *self = (UtX509Certificate *)object;
}

static UtObjectInterface object_interface = {
    .type_name = "UtX509Certificate", .cleanup = ut_x509_certificate_cleanup};

UtObject *ut_x509_certificate_new() {
  return ut_object_new(sizeof(UtX509Certificate), &object_interface);
}

bool ut_object_is_x509_certificate(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
