#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *values;
} UtObjectIdentifier;

static bool ut_object_identifier_equal(UtObject *object, UtObject *other) {
  UtObjectIdentifier *self = (UtObjectIdentifier *)object;
  UtObjectIdentifier *other_self = (UtObjectIdentifier *)other;
  if (!ut_object_is_object_identifier(other)) {
    return false;
  }

  return ut_object_equal(self->values, other_self->values);
}

static char *ut_object_identifier_to_object_string(UtObject *object) {
  ut_cstring_ref values_string = ut_object_identifier_to_string(object);
  return ut_cstring_new_printf("<UtObjectIdentifier>(\"%s\")", values_string);
}

static void ut_object_identifier_cleanup(UtObject *object) {
  UtObjectIdentifier *self = (UtObjectIdentifier *)object;
  ut_object_unref(self->values);
}

static UtObjectInterface object_interface = {
    .type_name = "UtObjectIdentifier",
    .equal = ut_object_identifier_equal,
    .to_string = ut_object_identifier_to_object_string,
    .cleanup = ut_object_identifier_cleanup};

UtObject *ut_object_identifier_new(UtObject *values) {
  UtObject *object =
      ut_object_new(sizeof(UtObjectIdentifier), &object_interface);
  UtObjectIdentifier *self = (UtObjectIdentifier *)object;

  self->values = ut_object_ref(values);

  return object;
}

UtObject *ut_object_identifier_new_from_string(const char *string) {
  UtObjectRef values = ut_uint32_list_new();
  const char *c = string;
  while (*c != 0) {
    uint32_t value = 0;
    do {
      if (*c < '0' || *c > '9') {
        return ut_error_new("Invalid object identifier string");
      }
      value = value * 10 + (*c - '0');
      c++;
    } while (*c != '.' && *c != '\0');
    ut_uint32_list_append(values, value);

    if (*c == '.') {
      c++;
    }
  }

  if (ut_list_get_length(values) < 2) {
    return ut_error_new("Invalid object identifier string");
  }

  return ut_object_identifier_new(values);
}

UtObject *ut_object_identifier_get_values(UtObject *object) {
  assert(ut_object_is_object_identifier(object));
  UtObjectIdentifier *self = (UtObjectIdentifier *)object;
  return self->values;
}

char *ut_object_identifier_to_string(UtObject *object) {
  assert(ut_object_is_object_identifier(object));
  UtObjectIdentifier *self = (UtObjectIdentifier *)object;

  UtObjectRef string = ut_string_new("");
  size_t values_length = ut_list_get_length(self->values);
  for (size_t i = 0; i < values_length; i++) {
    if (i != 0) {
      ut_string_append(string, ".");
    }
    ut_string_append_printf(string, "%d",
                            ut_uint32_list_get_element(self->values, i));
  }

  return ut_string_take_text(string);
}

bool ut_object_is_object_identifier(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
