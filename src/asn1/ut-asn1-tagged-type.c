#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  unsigned int number;
  UtObject *type;
} UtAsn1TaggedType;

static char *ut_asn1_tagged_type_to_string(UtObject *object) {
  UtAsn1TaggedType *self = (UtAsn1TaggedType *)object;
  ut_cstring_ref type_string = ut_object_to_string(self->type);
  return ut_cstring_new_printf("<UtAsn1TaggedType>(%u, %s)", self->number,
                               type_string);
}

static void ut_asn1_tagged_type_cleanup(UtObject *object) {
  UtAsn1TaggedType *self = (UtAsn1TaggedType *)object;
  ut_object_unref(self->type);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1TaggedType",
    .to_string = ut_asn1_tagged_type_to_string,
    .cleanup = ut_asn1_tagged_type_cleanup,
    .interfaces = {{NULL, NULL}}};

UtObject *ut_asn1_tagged_type_new(unsigned int number, UtObject *type) {
  UtObject *object = ut_object_new(sizeof(UtAsn1TaggedType), &object_interface);
  UtAsn1TaggedType *self = (UtAsn1TaggedType *)object;

  self->number = number;
  self->type = ut_object_ref(type);

  return object;
}

unsigned int ut_asn1_tagged_type_get_number(UtObject *object) {
  assert(ut_object_is_asn1_tagged_type(object));
  UtAsn1TaggedType *self = (UtAsn1TaggedType *)object;
  return self->number;
}

UtObject *ut_asn1_tagged_type_get_type(UtObject *object) {
  assert(ut_object_is_asn1_tagged_type(object));
  UtAsn1TaggedType *self = (UtAsn1TaggedType *)object;
  return self->type;
}

bool ut_object_is_asn1_tagged_type(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
