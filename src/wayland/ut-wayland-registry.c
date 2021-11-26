#include <assert.h>

#include "ut-wayland-client-private.h"
#include "ut-wayland-registry-private.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *client;
  uint32_t id;
  const UtWaylandRegistryCallbacks *callbacks;
  void *user_data;
} UtWaylandRegistry;

static void decode_global(UtWaylandRegistry *self, UtObject *data) {
  uint32_t name = ut_wayland_decoder_get_uint(data);
  ut_cstring_ref interface = ut_wayland_decoder_get_string(data);
  uint32_t version = ut_wayland_decoder_get_uint(data);
  if (self->callbacks != NULL && self->callbacks->global != NULL) {
    self->callbacks->global(self->user_data, name, interface, version);
  }
}

static void decode_global_remove(UtWaylandRegistry *self, UtObject *data) {
  uint32_t name = ut_wayland_decoder_get_uint(data);
  if (self->callbacks != NULL && self->callbacks->global_remove != NULL) {
    self->callbacks->global_remove(self->user_data, name);
  }
}

static const char *ut_wayland_registry_get_interface(UtObject *object) {
  return "wl_registry";
}

static uint32_t ut_wayland_registry_get_id(UtObject *object) {
  UtWaylandRegistry *self = (UtWaylandRegistry *)object;
  return self->id;
}

static bool ut_wayland_registry_decode_event(UtObject *object, uint16_t code,
                                             UtObject *data) {
  UtWaylandRegistry *self = (UtWaylandRegistry *)object;
  switch (code) {
  case 0:
    decode_global(self, data);
    return true;
  case 1:
    decode_global_remove(self, data);
    return true;
  default:
    return false;
  }
}

static UtWaylandObjectInterface wayland_object_interface = {
    .get_interface = ut_wayland_registry_get_interface,
    .get_id = ut_wayland_registry_get_id,
    .decode_event = ut_wayland_registry_decode_event};

static UtObjectInterface object_interface = {
    .type_name = "UtWaylandRegistry",
    .interfaces = {{&ut_wayland_object_id, &wayland_object_interface},
                   {NULL, NULL}}};

UtObject *ut_wayland_registry_new(UtObject *client, uint32_t id,
                                  const UtWaylandRegistryCallbacks *callbacks,
                                  void *user_data) {
  UtObject *object =
      ut_object_new(sizeof(UtWaylandRegistry), &object_interface);
  UtWaylandRegistry *self = (UtWaylandRegistry *)object;
  self->client = client;
  self->id = id;
  self->callbacks = callbacks;
  self->user_data = user_data;
  ut_wayland_client_register_object(client, object);
  return object;
}

UtObject *ut_wayland_registry_get_client(UtObject *object) {
  assert(ut_object_is_wayland_registry(object));
  UtWaylandRegistry *self = (UtWaylandRegistry *)object;
  return self->client;
}

uint32_t ut_wayland_registry_bind(UtObject *object, uint32_t name,
                                  const char *interface, uint32_t version) {
  assert(ut_object_is_wayland_registry(object));
  UtWaylandRegistry *self = (UtWaylandRegistry *)object;

  uint32_t id = ut_wayland_client_allocate_id(self->client);
  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_uint(payload, name);
  ut_wayland_encoder_append_string(payload, interface);
  ut_wayland_encoder_append_uint(payload, version);
  ut_wayland_encoder_append_uint(payload, id);
  ut_wayland_client_send_request(self->client, self->id, 0,
                                 ut_wayland_encoder_get_data(payload));
  return id;
}

bool ut_object_is_wayland_registry(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
