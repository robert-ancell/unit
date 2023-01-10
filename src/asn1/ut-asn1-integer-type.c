#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
} UtAsn1IntegerType;

static UtObjectInterface object_interface = {.type_name = "UtAsn1IntegerType",
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_asn1_integer_type_new() {
  return ut_object_new(sizeof(UtAsn1IntegerType), &object_interface);
}

bool ut_object_is_asn1_integer_type(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
