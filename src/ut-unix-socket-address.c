#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  char *path;
} UtUnixSocketAddress;

static void ut_unix_socket_address_cleanup(UtObject *object) {
  UtUnixSocketAddress *self = (UtUnixSocketAddress *)object;
  free(self->path);
}

static char *ut_unix_socket_address_to_string(UtObject *object) {
  UtUnixSocketAddress *self = (UtUnixSocketAddress *)object;
  return ut_cstring_new_printf("<UtUnixSocketAddress>(%s)", self->path);
}

static UtObjectInterface object_interface = {
    .type_name = "UtUnixSocketAddress",
    .cleanup = ut_unix_socket_address_cleanup,
    .to_string = ut_unix_socket_address_to_string,
    .interfaces = {{NULL, NULL}}};

UtObject *ut_unix_socket_address_new(const char *path) {
  UtObject *object =
      ut_object_new(sizeof(UtUnixSocketAddress), &object_interface);
  UtUnixSocketAddress *self = (UtUnixSocketAddress *)object;
  self->path = ut_cstring_new(path);
  return object;
}

const char *ut_unix_socket_address_get_path(UtObject *object) {
  assert(ut_object_is_unix_socket_address(object));
  UtUnixSocketAddress *self = (UtUnixSocketAddress *)object;
  return self->path;
}

bool ut_object_is_unix_socket_address(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
