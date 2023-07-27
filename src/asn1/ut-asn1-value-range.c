#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *lower_value;
  UtObject *upper_value;
} UtAsn1ValueRange;

static char *ut_asn1_value_range_to_string(UtObject *object) {
  UtAsn1ValueRange *self = (UtAsn1ValueRange *)object;
  ut_cstring_ref lower_value_string = ut_object_to_string(self->lower_value);
  ut_cstring_ref upper_value_string = ut_object_to_string(self->upper_value);
  return ut_cstring_new_printf("<UtAsn1ValueRange>(%s, %s)", lower_value_string,
                               upper_value_string);
}

static void ut_asn1_value_range_cleanup(UtObject *object) {
  UtAsn1ValueRange *self = (UtAsn1ValueRange *)object;
  ut_object_unref(self->lower_value);
  ut_object_unref(self->upper_value);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1ValueRange",
    .to_string = ut_asn1_value_range_to_string,
    .cleanup = ut_asn1_value_range_cleanup};

UtObject *ut_asn1_value_range_new(UtObject *lower_value,
                                  UtObject *upper_value) {
  UtObject *object = ut_object_new(sizeof(UtAsn1ValueRange), &object_interface);
  UtAsn1ValueRange *self = (UtAsn1ValueRange *)object;

  self->lower_value = ut_object_ref(lower_value);
  self->upper_value = ut_object_ref(upper_value);

  return object;
}

UtObject *ut_asn1_value_range_get_lower_value(UtObject *object) {
  assert(ut_object_is_asn1_value_range(object));
  UtAsn1ValueRange *self = (UtAsn1ValueRange *)object;
  return self->lower_value;
}

UtObject *ut_asn1_value_range_get_upper_value(UtObject *object) {
  assert(ut_object_is_asn1_value_range(object));
  UtAsn1ValueRange *self = (UtAsn1ValueRange *)object;
  return self->upper_value;
}

bool ut_object_is_asn1_value_range(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
