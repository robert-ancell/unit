#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  char *description;
} UtAsn1Error;

static char *ut_asn1_error_to_string(UtObject *object) {
  UtAsn1Error *self = (UtAsn1Error *)object;
  return ut_cstring_new_printf("<UtAsn1Error>(\"%s\")", self->description);
}

static void ut_asn1_error_cleanup(UtObject *object) {
  UtAsn1Error *self = (UtAsn1Error *)object;
  free(self->description);
}

static char *ut_asn1_error_get_description(UtObject *object) {
  UtAsn1Error *self = (UtAsn1Error *)object;
  return ut_cstring_new(self->description);
}

static UtErrorInterface error_interface = {.get_description =
                                               ut_asn1_error_get_description};

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1Error",
    .to_string = ut_asn1_error_to_string,
    .cleanup = ut_asn1_error_cleanup,
    .interfaces = {{&ut_error_id, &error_interface}, {NULL, NULL}}};

UtObject *ut_asn1_error_new(const char *description) {
  UtObject *object = ut_object_new(sizeof(UtAsn1Error), &object_interface);
  UtAsn1Error *self = (UtAsn1Error *)object;
  self->description = ut_cstring_new(description);
  return object;
}

bool ut_object_is_asn1_error(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
