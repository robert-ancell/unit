#include <assert.h>
#include <stdlib.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *value;
} UtDBusVariant;

static char *ut_dbus_variant_to_string(UtObject *object) {
  UtDBusVariant *self = (UtDBusVariant *)object;
  ut_cstring_ref value_string = ut_object_to_string(self->value);
  return ut_cstring_new_printf("<UtDBusVariant>(%s)", value_string);
}

static void ut_dbus_variant_cleanup(UtObject *object) {
  UtDBusVariant *self = (UtDBusVariant *)object;
  ut_object_unref(self->value);
}

static UtObjectInterface object_interface = {.type_name = "UtDBusVariant",
                                             .to_string =
                                                 ut_dbus_variant_to_string,
                                             .cleanup = ut_dbus_variant_cleanup,
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_dbus_variant_new(UtObject *value) {
  return ut_dbus_variant_new_take(ut_object_ref(value));
}

UtObject *ut_dbus_variant_new_take(UtObject *value) {
  UtObject *object = ut_object_new(sizeof(UtDBusVariant), &object_interface);
  UtDBusVariant *self = (UtDBusVariant *)object;
  self->value = value;
  return object;
}

UtObject *ut_dbus_variant_get_value(UtObject *object) {
  assert(ut_object_is_dbus_variant(object));
  UtDBusVariant *self = (UtDBusVariant *)object;
  return self->value;
}

bool ut_object_is_dbus_variant(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
