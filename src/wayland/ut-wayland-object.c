#include <assert.h>

#include "ut.h"

int ut_wayland_object_id = 0;

const char *ut_wayland_object_get_interface(UtObject *object) {
  UtWaylandObjectInterface *wayland_object_interface =
      ut_object_get_interface(object, &ut_wayland_object_id);
  assert(wayland_object_interface != NULL);
  return wayland_object_interface->get_interface(object);
}

uint32_t ut_wayland_object_get_id(UtObject *object) {
  UtWaylandObjectInterface *wayland_object_interface =
      ut_object_get_interface(object, &ut_wayland_object_id);
  assert(wayland_object_interface != NULL);
  return wayland_object_interface->get_id(object);
}

bool ut_wayland_object_decode_event(UtObject *object, uint16_t code,
                                    UtObject *data) {
  UtWaylandObjectInterface *wayland_object_interface =
      ut_object_get_interface(object, &ut_wayland_object_id);
  assert(wayland_object_interface != NULL);
  if (wayland_object_interface->decode_event != NULL) {
    return wayland_object_interface->decode_event(object, code, data);
  } else {
    return false;
  }
}

bool ut_object_implements_wayland_object(UtObject *object) {
  return ut_object_get_interface(object, &ut_wayland_object_id) != NULL;
}
