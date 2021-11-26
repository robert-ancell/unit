#include <assert.h>

#include "ut-wayland-client-private.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *client;
  uint32_t id;
  const UtXdgSurfaceCallbacks *callbacks;
  void *user_data;
} UtXdgSurface;

static void decode_configure(UtXdgSurface *self, UtObject *data) {
  uint32_t serial = ut_wayland_decoder_get_uint(data);
  if (self->callbacks != NULL && self->callbacks->configure != NULL) {
    self->callbacks->configure(self->user_data, serial);
  }
}

static const char *ut_xdg_surface_get_interface(UtObject *object) {
  return "xdg_surface";
}

static uint32_t ut_xdg_surface_get_id(UtObject *object) {
  UtXdgSurface *self = (UtXdgSurface *)object;
  return self->id;
}

static bool ut_xdg_surface_decode_event(UtObject *object, uint16_t code,
                                        UtObject *data) {
  UtXdgSurface *self = (UtXdgSurface *)object;
  switch (code) {
  case 0:
    decode_configure(self, data);
    return true;
  default:
    return false;
  }
}

static UtWaylandObjectInterface wayland_object_interface = {
    .get_interface = ut_xdg_surface_get_interface,
    .get_id = ut_xdg_surface_get_id,
    .decode_event = ut_xdg_surface_decode_event};

static UtObjectInterface object_interface = {
    .type_name = "UtXdgSurface",
    .interfaces = {{&ut_wayland_object_id, &wayland_object_interface},
                   {NULL, NULL}}};

UtObject *ut_xdg_surface_new(UtObject *client, uint32_t id,
                             const UtXdgSurfaceCallbacks *callbacks,
                             void *user_data) {
  UtObject *object = ut_object_new(sizeof(UtXdgSurface), &object_interface);
  UtXdgSurface *self = (UtXdgSurface *)object;
  self->client = client;
  self->id = id;
  self->callbacks = callbacks;
  self->user_data = user_data;
  ut_wayland_client_register_object(client, object);
  return object;
}

void ut_xdg_surface_destroy(UtObject *object) {
  assert(ut_object_is_xdg_surface(object));
  UtXdgSurface *self = (UtXdgSurface *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_client_send_request(self->client, self->id, 0,
                                 ut_wayland_encoder_get_data(payload));
}

UtObject *ut_xdg_surface_get_toplevel(UtObject *object,
                                      const UtXdgToplevelCallbacks *callbacks,
                                      void *user_data) {
  assert(ut_object_is_xdg_surface(object));
  UtXdgSurface *self = (UtXdgSurface *)object;

  uint32_t id = ut_wayland_client_allocate_id(self->client);
  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_uint(payload, id);
  ut_wayland_client_send_request(self->client, self->id, 1,
                                 ut_wayland_encoder_get_data(payload));
  return ut_xdg_toplevel_new(self->client, id, callbacks, user_data);
}

UtObject *ut_xdg_surface_get_popup(UtObject *object, UtObject *parent,
                                   UtObject *positioner,
                                   const UtXdgPopupCallbacks *callbacks,
                                   void *user_data) {
  assert(ut_object_is_xdg_surface(object));
  UtXdgSurface *self = (UtXdgSurface *)object;

  uint32_t id = ut_wayland_client_allocate_id(self->client);
  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_uint(payload, id);
  ut_wayland_encoder_append_object(payload, parent);
  ut_wayland_encoder_append_object(payload, positioner);
  ut_wayland_client_send_request(self->client, self->id, 2,
                                 ut_wayland_encoder_get_data(payload));
  return ut_xdg_popup_new(self->client, id, callbacks, user_data);
}

void ut_xdg_surface_set_window_geometry(UtObject *object, int32_t x, int32_t y,
                                        int32_t width, int32_t height) {
  assert(ut_object_is_xdg_surface(object));
  UtXdgSurface *self = (UtXdgSurface *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_int(payload, x);
  ut_wayland_encoder_append_int(payload, y);
  ut_wayland_encoder_append_int(payload, width);
  ut_wayland_encoder_append_int(payload, height);
  ut_wayland_client_send_request(self->client, self->id, 3,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_xdg_surface_ack_configure(UtObject *object, uint32_t serial) {
  assert(ut_object_is_xdg_surface(object));
  UtXdgSurface *self = (UtXdgSurface *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_uint(payload, serial);
  ut_wayland_client_send_request(self->client, self->id, 4,
                                 ut_wayland_encoder_get_data(payload));
}

bool ut_object_is_xdg_surface(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
