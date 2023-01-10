#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
} UtAsn1BooleanType;

static UtObjectInterface object_interface = {.type_name = "UtAsn1BooleanType",
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_asn1_boolean_type_new() {
  return ut_object_new(sizeof(UtAsn1BooleanType), &object_interface);
}

bool ut_object_is_asn1_boolean_type(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
