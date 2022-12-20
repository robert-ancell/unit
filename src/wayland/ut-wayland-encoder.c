#include <assert.h>
#include <string.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *data;
  UtObject *fds;
  UtObject *data_with_fds;
} UtWaylandEncoder;

static void ut_wayland_encoder_init(UtObject *object) {
  UtWaylandEncoder *self = (UtWaylandEncoder *)object;
  self->data = ut_uint8_array_new();
  self->fds = ut_object_list_new();
  self->data_with_fds = ut_uint8_array_with_fds_new(self->data, self->fds);
}

static void ut_wayland_encoder_cleanup(UtObject *object) {
  UtWaylandEncoder *self = (UtWaylandEncoder *)object;
  ut_object_unref(self->data);
  ut_object_unref(self->fds);
  ut_object_unref(self->data_with_fds);
}

static UtObjectInterface object_interface = {.type_name = "UtWaylandEncoder",
                                             .init = ut_wayland_encoder_init,
                                             .cleanup =
                                                 ut_wayland_encoder_cleanup,
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_wayland_encoder_new() {
  return ut_object_new(sizeof(UtWaylandEncoder), &object_interface);
}

void ut_wayland_encoder_append_uint(UtObject *object, uint32_t value) {
  assert(ut_object_is_wayland_encoder(object));
  UtWaylandEncoder *self = (UtWaylandEncoder *)object;

  // FIXME: Write at native endian
  ut_uint8_list_append_uint32_le(self->data, value);
}

void ut_wayland_encoder_append_int(UtObject *object, int32_t value) {
  ut_wayland_encoder_append_uint(object, (uint32_t)value);
}

void ut_wayland_encoder_append_string(UtObject *object, const char *value) {
  assert(ut_object_is_wayland_encoder(object));
  UtWaylandEncoder *self = (UtWaylandEncoder *)object;

  size_t value_length = strlen(value) + 1;
  ut_wayland_encoder_append_uint(object, value_length);
  for (size_t i = 0; i < value_length; i++) {
    ut_uint8_list_append(self->data, value[i]);
  }
  for (size_t i = value_length; i % 4 != 0; i++) {
    ut_uint8_list_append(self->data, 0);
  }
}

void ut_wayland_encoder_append_object(UtObject *object,
                                      UtObject *wayland_object) {
  assert(ut_object_implements_wayland_object(wayland_object));
  ut_wayland_encoder_append_uint(object,
                                 ut_wayland_object_get_id(wayland_object));
}

void ut_wayland_encoder_append_fd(UtObject *object, UtObject *fd) {
  assert(ut_object_is_wayland_encoder(object));
  UtWaylandEncoder *self = (UtWaylandEncoder *)object;
  ut_list_append(self->fds, fd);
}

UtObject *ut_wayland_encoder_get_data(UtObject *object) {
  assert(ut_object_is_wayland_encoder(object));
  UtWaylandEncoder *self = (UtWaylandEncoder *)object;
  if (ut_list_get_length(self->fds) > 0) {
    return self->data_with_fds;
  } else {
    return self->data;
  }
}

bool ut_object_is_wayland_encoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
