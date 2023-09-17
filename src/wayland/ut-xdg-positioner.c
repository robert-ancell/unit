#include <assert.h>

#include "ut-wayland-client-private.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *client;
  uint32_t id;
} UtXdgPositioner;

static const char *ut_xdg_positioner_get_interface(UtObject *object) {
  return "xdg_positioner";
}

static uint32_t ut_xdg_positioner_get_id(UtObject *object) {
  UtXdgPositioner *self = (UtXdgPositioner *)object;
  return self->id;
}

static UtWaylandObjectInterface wayland_object_interface = {
    .get_interface = ut_xdg_positioner_get_interface,
    .get_id = ut_xdg_positioner_get_id};

static UtObjectInterface object_interface = {
    .type_name = "UtXdgPositioner",
    .interfaces = {{&ut_wayland_object_id, &wayland_object_interface},
                   {NULL, NULL}}};

UtObject *ut_xdg_positioner_new(UtObject *client, uint32_t id) {
  UtObject *object = ut_object_new(sizeof(UtXdgPositioner), &object_interface);
  UtXdgPositioner *self = (UtXdgPositioner *)object;
  self->client = client;
  self->id = id;
  ut_wayland_client_register_object(client, object);
  return object;
}

void ut_xdg_positioner_destroy(UtObject *object) {
  assert(ut_object_is_xdg_positioner(object));
  UtXdgPositioner *self = (UtXdgPositioner *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_client_send_request(self->client, self->id, 0,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_xdg_positioner_set_size(UtObject *object, int32_t width,
                                int32_t height) {
  assert(ut_object_is_xdg_positioner(object));
  UtXdgPositioner *self = (UtXdgPositioner *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_int(payload, width);
  ut_wayland_encoder_append_int(payload, height);
  ut_wayland_client_send_request(self->client, self->id, 1,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_xdg_positioner_set_anchor_rect(UtObject *object, int32_t x, int32_t y,
                                       int32_t width, int32_t height) {
  assert(ut_object_is_xdg_positioner(object));
  UtXdgPositioner *self = (UtXdgPositioner *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_int(payload, x);
  ut_wayland_encoder_append_int(payload, y);
  ut_wayland_encoder_append_int(payload, width);
  ut_wayland_encoder_append_int(payload, height);
  ut_wayland_client_send_request(self->client, self->id, 2,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_xdg_positioner_set_anchor(UtObject *object, uint32_t anchor) {
  assert(ut_object_is_xdg_positioner(object));
  UtXdgPositioner *self = (UtXdgPositioner *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_uint(payload, anchor);
  ut_wayland_client_send_request(self->client, self->id, 3,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_xdg_positioner_set_gravity(UtObject *object, uint32_t gravity) {
  assert(ut_object_is_xdg_positioner(object));
  UtXdgPositioner *self = (UtXdgPositioner *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_uint(payload, gravity);
  ut_wayland_client_send_request(self->client, self->id, 4,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_xdg_positioner_set_constraint_adjustment(
    UtObject *object, uint32_t constraint_adjustment) {
  assert(ut_object_is_xdg_positioner(object));
  UtXdgPositioner *self = (UtXdgPositioner *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_uint(payload, constraint_adjustment);
  ut_wayland_client_send_request(self->client, self->id, 5,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_xdg_positioner_set_offset(UtObject *object, int32_t x, int32_t y) {
  assert(ut_object_is_xdg_positioner(object));
  UtXdgPositioner *self = (UtXdgPositioner *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_int(payload, x);
  ut_wayland_encoder_append_int(payload, y);
  ut_wayland_client_send_request(self->client, self->id, 6,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_xdg_positioner_set_reactive(UtObject *object) {
  assert(ut_object_is_xdg_positioner(object));
  UtXdgPositioner *self = (UtXdgPositioner *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_client_send_request(self->client, self->id, 7,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_xdg_positioner_set_parent_size(UtObject *object, int32_t width,
                                       int32_t height) {
  assert(ut_object_is_xdg_positioner(object));
  UtXdgPositioner *self = (UtXdgPositioner *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_int(payload, width);
  ut_wayland_encoder_append_int(payload, height);
  ut_wayland_client_send_request(self->client, self->id, 8,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_xdg_positioner_set_parent_configure(UtObject *object, uint32_t serial) {
  assert(ut_object_is_xdg_positioner(object));
  UtXdgPositioner *self = (UtXdgPositioner *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_client_send_request(self->client, self->id, 9,
                                 ut_wayland_encoder_get_data(payload));
}

bool ut_object_is_xdg_positioner(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
