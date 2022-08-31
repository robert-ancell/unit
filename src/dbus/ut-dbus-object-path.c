#include <assert.h>
#include <stdlib.h>

#include "ut.h"

typedef struct {
  UtObject object;
  char *value;
} UtDBusObjectPath;

static char *ut_dbus_object_path_to_string(UtObject *object) {
  UtDBusObjectPath *self = (UtDBusObjectPath *)object;
  return ut_cstring_new_printf("<UtDBusObjectPath>(\"%s\")", self->value);
}

static void ut_dbus_object_path_cleanup(UtObject *object) {
  UtDBusObjectPath *self = (UtDBusObjectPath *)object;
  free(self->value);
}

static UtObjectInterface object_interface = {
    .type_name = "UtDBusObjectPath",
    .to_string = ut_dbus_object_path_to_string,
    .cleanup = ut_dbus_object_path_cleanup,
    .interfaces = {{NULL, NULL}}};

UtObject *ut_dbus_object_path_new(const char *value) {
  UtObject *object = ut_object_new(sizeof(UtDBusObjectPath), &object_interface);
  UtDBusObjectPath *self = (UtDBusObjectPath *)object;
  self->value = ut_cstring_new(value);
  return object;
}

const char *ut_dbus_object_path_get_value(UtObject *object) {
  assert(ut_object_is_dbus_object_path(object));
  UtDBusObjectPath *self = (UtDBusObjectPath *)object;
  return self->value;
}

bool ut_object_is_dbus_object_path(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
