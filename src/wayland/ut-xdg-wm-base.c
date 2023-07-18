#include <assert.h>

#include "ut-wayland-client-private.h"
#include "ut-wayland-registry-private.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *client;
  uint32_t id;
  UtObject *callback_object;
  const UtXdgWmBaseCallbacks *callbacks;
} UtXdgWmBase;

static void decode_ping(UtXdgWmBase *self, UtObject *data) {
  uint32_t serial = ut_wayland_decoder_get_uint(data);
  if (self->callback_object != NULL && self->callbacks != NULL &&
      self->callbacks->ping != NULL) {
    self->callbacks->ping(self->callback_object, serial);
  }
}

static const char *ut_xdg_wm_base_get_interface(UtObject *object) {
  return "xdg_wm_base";
}

static uint32_t ut_xdg_wm_base_get_id(UtObject *object) {
  UtXdgWmBase *self = (UtXdgWmBase *)object;
  return self->id;
}

static bool ut_xdg_wm_base_decode_event(UtObject *object, uint16_t code,
                                        UtObject *data) {
  UtXdgWmBase *self = (UtXdgWmBase *)object;
  switch (code) {
  case 0:
    decode_ping(self, data);
    return true;
  default:
    return false;
  }
}

static void ut_xdg_wm_base_cleanup(UtObject *object) {
  UtXdgWmBase *self = (UtXdgWmBase *)object;
  ut_object_weak_unref(&self->callback_object);
}

static UtWaylandObjectInterface wayland_object_interface = {
    .get_interface = ut_xdg_wm_base_get_interface,
    .get_id = ut_xdg_wm_base_get_id,
    .decode_event = ut_xdg_wm_base_decode_event};

static UtObjectInterface object_interface = {
    .type_name = "UtXdgWmBase",
    .cleanup = ut_xdg_wm_base_cleanup,
    .interfaces = {{&ut_wayland_object_id, &wayland_object_interface},
                   {NULL, NULL}}};

UtObject *ut_xdg_wm_base_new(UtObject *client, uint32_t id,
                             UtObject *callback_object,
                             const UtXdgWmBaseCallbacks *callbacks) {
  UtObject *object = ut_object_new(sizeof(UtXdgWmBase), &object_interface);
  UtXdgWmBase *self = (UtXdgWmBase *)object;
  self->client = client;
  self->id = id;
  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callbacks = callbacks;
  ut_wayland_client_register_object(client, object);
  return object;
}

UtObject *
ut_xdg_wm_base_new_from_registry(UtObject *registry, uint32_t name,
                                 UtObject *callback_object,
                                 const UtXdgWmBaseCallbacks *callbacks) {
  uint32_t id = ut_wayland_registry_bind(registry, name, "xdg_wm_base", 4);
  return ut_xdg_wm_base_new(ut_wayland_registry_get_client(registry), id,
                            callback_object, callbacks);
}

void ut_xdg_wm_base_destroy(UtObject *object) {
  assert(ut_object_is_xdg_wm_base(object));
  UtXdgWmBase *self = (UtXdgWmBase *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_client_send_request(self->client, self->id, 0,
                                 ut_wayland_encoder_get_data(payload));
}

UtObject *ut_xdg_wm_base_create_positioner(UtObject *object) {
  assert(ut_object_is_xdg_wm_base(object));
  UtXdgWmBase *self = (UtXdgWmBase *)object;

  uint32_t id = ut_wayland_client_allocate_id(self->client);
  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_uint(payload, id);
  ut_wayland_client_send_request(self->client, self->id, 1,
                                 ut_wayland_encoder_get_data(payload));
  return ut_xdg_positioner_new(self->client, id);
}

UtObject *
ut_xdg_wm_base_get_xdg_surface(UtObject *object, UtObject *surface,
                               UtObject *callback_object,
                               const UtXdgSurfaceCallbacks *callbacks) {
  assert(ut_object_is_xdg_wm_base(object));
  assert(ut_object_is_wayland_surface(surface));
  UtXdgWmBase *self = (UtXdgWmBase *)object;

  uint32_t id = ut_wayland_client_allocate_id(self->client);
  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_uint(payload, id);
  ut_wayland_encoder_append_object(payload, surface);
  ut_wayland_client_send_request(self->client, self->id, 2,
                                 ut_wayland_encoder_get_data(payload));
  return ut_xdg_surface_new(self->client, id, callback_object, callbacks);
}

void ut_xdg_wm_base_pong(UtObject *object, uint32_t serial) {
  assert(ut_object_is_xdg_wm_base(object));
  UtXdgWmBase *self = (UtXdgWmBase *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_uint(payload, serial);
  ut_wayland_client_send_request(self->client, self->id, 3,
                                 ut_wayland_encoder_get_data(payload));
}

bool ut_object_is_xdg_wm_base(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
