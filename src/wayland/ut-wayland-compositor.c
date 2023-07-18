#include <assert.h>

#include "ut-wayland-client-private.h"
#include "ut-wayland-registry-private.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *client;
  uint32_t id;
} UtWaylandCompositor;

static const char *ut_wayland_compositor_get_interface(UtObject *object) {
  return "wl_compositor";
}

static uint32_t ut_wayland_compositor_get_id(UtObject *object) {
  UtWaylandCompositor *self = (UtWaylandCompositor *)object;
  return self->id;
}

static UtWaylandObjectInterface wayland_object_interface = {
    .get_interface = ut_wayland_compositor_get_interface,
    .get_id = ut_wayland_compositor_get_id};

static UtObjectInterface object_interface = {
    .type_name = "UtWaylandCompositor",
    .interfaces = {{&ut_wayland_object_id, &wayland_object_interface},
                   {NULL, NULL}}};

UtObject *ut_wayland_compositor_new(UtObject *client, uint32_t id) {
  UtObject *object =
      ut_object_new(sizeof(UtWaylandCompositor), &object_interface);
  UtWaylandCompositor *self = (UtWaylandCompositor *)object;
  self->client = client;
  self->id = id;
  ut_wayland_client_register_object(client, object);
  return object;
}

UtObject *ut_wayland_compositor_new_from_registry(UtObject *registry,
                                                  uint32_t name) {
  uint32_t id = ut_wayland_registry_bind(registry, name, "wl_compositor", 5);
  return ut_wayland_compositor_new(ut_wayland_registry_get_client(registry),
                                   id);
}

UtObject *ut_wayland_compositor_create_surface(
    UtObject *object, UtObject *callback_object,
    const UtWaylandSurfaceCallbacks *callbacks) {
  assert(ut_object_is_wayland_compositor(object));
  UtWaylandCompositor *self = (UtWaylandCompositor *)object;

  uint32_t id = ut_wayland_client_allocate_id(self->client);
  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_uint(payload, id);
  ut_wayland_client_send_request(self->client, self->id, 0,
                                 ut_wayland_encoder_get_data(payload));
  UtObject *surface =
      ut_wayland_surface_new(self->client, id, callback_object, callbacks);
  ut_wayland_client_register_object(self->client, surface);
  return surface;
}

UtObject *ut_wayland_compositor_create_region(UtObject *object) {
  assert(ut_object_is_wayland_compositor(object));
  UtWaylandCompositor *self = (UtWaylandCompositor *)object;

  uint32_t id = ut_wayland_client_allocate_id(self->client);
  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_uint(payload, id);
  ut_wayland_client_send_request(self->client, self->id, 1,
                                 ut_wayland_encoder_get_data(payload));
  UtObject *region = ut_wayland_region_new(self->client, id);
  ut_wayland_client_register_object(self->client, region);
  return region;
}

bool ut_object_is_wayland_compositor(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
