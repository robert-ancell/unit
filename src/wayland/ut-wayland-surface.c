#include <assert.h>

#include "ut-wayland-client-private.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *client;
  uint32_t id;
  UtObject *callback_object;
  const UtWaylandSurfaceCallbacks *callbacks;
} UtWaylandSurface;

static void decode_enter(UtWaylandSurface *self, UtObject *data) {
  uint32_t output_id = ut_wayland_decoder_get_uint(data);
  UtObject *output = ut_wayland_client_get_object(self->client, output_id);
  if (self->callback_object != NULL && self->callbacks != NULL &&
      self->callbacks->enter != NULL) {
    self->callbacks->enter(self->callback_object, output);
  }
}

static void decode_leave(UtWaylandSurface *self, UtObject *data) {
  uint32_t output_id = ut_wayland_decoder_get_uint(data);
  UtObject *output = ut_wayland_client_get_object(self->client, output_id);
  if (self->callback_object != NULL && self->callbacks != NULL &&
      self->callbacks->leave != NULL) {
    self->callbacks->leave(self->callback_object, output);
  }
}

static const char *ut_wayland_surface_get_interface(UtObject *object) {
  return "wl_surface";
}

static uint32_t ut_wayland_surface_get_id(UtObject *object) {
  UtWaylandSurface *self = (UtWaylandSurface *)object;
  return self->id;
}

static bool ut_wayland_surface_decode_event(UtObject *object, uint16_t code,
                                            UtObject *data) {
  UtWaylandSurface *self = (UtWaylandSurface *)object;
  switch (code) {
  case 0:
    decode_enter(self, data);
    return true;
  case 1:
    decode_leave(self, data);
    return true;
  default:
    return false;
  }
}

static void ut_xdg_wayland_surface_cleanup(UtObject *object) {
  UtWaylandSurface *self = (UtWaylandSurface *)object;
  ut_object_weak_unref(&self->callback_object);
}

static UtWaylandObjectInterface wayland_object_interface = {
    .get_interface = ut_wayland_surface_get_interface,
    .get_id = ut_wayland_surface_get_id,
    .decode_event = ut_wayland_surface_decode_event};

static UtObjectInterface object_interface = {
    .type_name = "UtWaylandSurface",
    .cleanup = ut_xdg_wayland_surface_cleanup,
    .interfaces = {{&ut_wayland_object_id, &wayland_object_interface},
                   {NULL, NULL}}};

UtObject *ut_wayland_surface_new(UtObject *client, uint32_t id,
                                 UtObject *callback_object,
                                 const UtWaylandSurfaceCallbacks *callbacks) {
  UtObject *object = ut_object_new(sizeof(UtWaylandSurface), &object_interface);
  UtWaylandSurface *self = (UtWaylandSurface *)object;
  self->client = client;
  self->id = id;
  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callbacks = callbacks;
  ut_wayland_client_register_object(client, object);
  return object;
}

void ut_wayland_surface_destroy(UtObject *object) {
  assert(ut_object_is_wayland_surface(object));
  UtWaylandSurface *self = (UtWaylandSurface *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_client_send_request(self->client, self->id, 0,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_wayland_surface_attach(UtObject *object, UtObject *buffer, int32_t x,
                               int32_t y) {
  assert(ut_object_is_wayland_surface(object));
  assert(ut_object_is_wayland_buffer(buffer));
  UtWaylandSurface *self = (UtWaylandSurface *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_object(payload, buffer);
  ut_wayland_encoder_append_int(payload, x);
  ut_wayland_encoder_append_int(payload, y);
  ut_wayland_client_send_request(self->client, self->id, 1,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_wayland_surface_damage(UtObject *object, int32_t x, int32_t y,
                               int32_t width, int32_t height) {
  assert(ut_object_is_wayland_surface(object));
  UtWaylandSurface *self = (UtWaylandSurface *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_int(payload, x);
  ut_wayland_encoder_append_int(payload, y);
  ut_wayland_encoder_append_int(payload, width);
  ut_wayland_encoder_append_int(payload, height);
  ut_wayland_client_send_request(self->client, self->id, 2,
                                 ut_wayland_encoder_get_data(payload));
}

UtObject *
ut_wayland_surface_frame(UtObject *object, UtObject *callback_object,
                         UtWaylandCallbackDoneCallback done_callback) {
  assert(ut_object_is_wayland_surface(object));
  UtWaylandSurface *self = (UtWaylandSurface *)object;

  uint32_t callback = ut_wayland_client_allocate_id(self->client);
  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_uint(payload, callback);
  ut_wayland_client_send_request(self->client, self->id, 3,
                                 ut_wayland_encoder_get_data(payload));
  return ut_wayland_callback_new(self->client, callback, callback_object,
                                 done_callback);
}

void ut_wayland_surface_set_opaque_region(UtObject *object, UtObject *region) {
  assert(ut_object_is_wayland_surface(object));
  assert(ut_object_is_wayland_region(region));
  UtWaylandSurface *self = (UtWaylandSurface *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_object(payload, region);
  ut_wayland_client_send_request(self->client, self->id, 4,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_wayland_surface_set_input_region(UtObject *object, UtObject *region) {
  assert(ut_object_is_wayland_surface(object));
  assert(ut_object_is_wayland_region(region));
  UtWaylandSurface *self = (UtWaylandSurface *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_object(payload, region);
  ut_wayland_client_send_request(self->client, self->id, 5,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_wayland_surface_commit(UtObject *object) {
  assert(ut_object_is_wayland_surface(object));
  UtWaylandSurface *self = (UtWaylandSurface *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_client_send_request(self->client, self->id, 6,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_wayland_surface_set_buffer_transform(UtObject *object,
                                             int32_t transform) {
  assert(ut_object_is_wayland_surface(object));
  UtWaylandSurface *self = (UtWaylandSurface *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_int(payload, transform);
  ut_wayland_client_send_request(self->client, self->id, 7,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_wayland_surface_set_buffer_scale(UtObject *object, int32_t scale) {
  assert(ut_object_is_wayland_surface(object));
  UtWaylandSurface *self = (UtWaylandSurface *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_int(payload, scale);
  ut_wayland_client_send_request(self->client, self->id, 8,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_wayland_surface_damage_buffer(UtObject *object, int32_t x, int32_t y,
                                      int32_t width, int32_t height) {
  assert(ut_object_is_wayland_surface(object));
  UtWaylandSurface *self = (UtWaylandSurface *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_int(payload, x);
  ut_wayland_encoder_append_int(payload, y);
  ut_wayland_encoder_append_int(payload, width);
  ut_wayland_encoder_append_int(payload, height);
  ut_wayland_client_send_request(self->client, self->id, 9,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_wayland_surface_offset(UtObject *object, int32_t x, int32_t y) {
  assert(ut_object_is_wayland_surface(object));
  UtWaylandSurface *self = (UtWaylandSurface *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_int(payload, x);
  ut_wayland_encoder_append_int(payload, y);
  ut_wayland_client_send_request(self->client, self->id, 10,
                                 ut_wayland_encoder_get_data(payload));
}

bool ut_object_is_wayland_surface(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
