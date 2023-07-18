#include <assert.h>

#include "ut-wayland-client-private.h"
#include "ut-wayland-registry-private.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *client;
  uint32_t id;
  UtObject *callback_object;
  const UtWaylandSeatCallbacks *callbacks;
} UtWaylandSeat;

static void decode_capabilities(UtWaylandSeat *self, UtObject *data) {
  uint32_t capabilities = ut_wayland_decoder_get_uint(data);
  if (self->callback_object != NULL && self->callbacks != NULL &&
      self->callbacks->capabilities != NULL) {
    self->callbacks->capabilities(self->callback_object, capabilities);
  }
}

static void decode_name(UtWaylandSeat *self, UtObject *data) {
  ut_cstring_ref name = ut_wayland_decoder_get_string(data);
  if (self->callback_object != NULL && self->callbacks != NULL &&
      self->callbacks->name != NULL) {
    self->callbacks->name(self->callback_object, name);
  }
}

static const char *ut_wayland_seat_get_interface(UtObject *object) {
  return "wl_seat";
}

static uint32_t ut_wayland_seat_get_id(UtObject *object) {
  UtWaylandSeat *self = (UtWaylandSeat *)object;
  return self->id;
}

static bool ut_wayland_seat_decode_event(UtObject *object, uint16_t code,
                                         UtObject *data) {
  UtWaylandSeat *self = (UtWaylandSeat *)object;
  switch (code) {
  case 0:
    decode_capabilities(self, data);
    return true;
  case 1:
    decode_name(self, data);
    return true;
  default:
    return false;
  }
}

static void ut_xdg_wayland_seat_cleanup(UtObject *object) {
  UtWaylandSeat *self = (UtWaylandSeat *)object;
  ut_object_weak_unref(&self->callback_object);
}

static UtWaylandObjectInterface wayland_object_interface = {
    .get_interface = ut_wayland_seat_get_interface,
    .get_id = ut_wayland_seat_get_id,
    .decode_event = ut_wayland_seat_decode_event};

static UtObjectInterface object_interface = {
    .type_name = "UtWaylandSeat",
    .cleanup = ut_xdg_wayland_seat_cleanup,
    .interfaces = {{&ut_wayland_object_id, &wayland_object_interface},
                   {NULL, NULL}}};

UtObject *ut_wayland_seat_new(UtObject *client, uint32_t id,
                              UtObject *callback_object,
                              const UtWaylandSeatCallbacks *callbacks) {
  UtObject *object = ut_object_new(sizeof(UtWaylandSeat), &object_interface);
  UtWaylandSeat *self = (UtWaylandSeat *)object;
  self->client = client;
  self->id = id;
  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callbacks = callbacks;
  ut_wayland_client_register_object(client, object);
  return object;
}

UtObject *
ut_wayland_seat_new_from_registry(UtObject *registry, uint32_t name,
                                  UtObject *callback_object,
                                  const UtWaylandSeatCallbacks *callbacks) {
  uint32_t id = ut_wayland_registry_bind(registry, name, "wl_seat", 8);
  return ut_wayland_seat_new(ut_wayland_registry_get_client(registry), id,
                             callback_object, callbacks);
}

UtObject *
ut_wayland_seat_get_pointer(UtObject *object, UtObject *callback_object,
                            const UtWaylandPointerCallbacks *callbacks) {
  assert(ut_object_is_wayland_seat(object));
  UtWaylandSeat *self = (UtWaylandSeat *)object;

  uint32_t id = ut_wayland_client_allocate_id(self->client);
  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_uint(payload, id);
  ut_wayland_client_send_request(self->client, self->id, 0,
                                 ut_wayland_encoder_get_data(payload));
  return ut_wayland_pointer_new(self->client, id, callback_object, callbacks);
}

UtObject *
ut_wayland_seat_get_keyboard(UtObject *object, UtObject *callback_object,
                             const UtWaylandKeyboardCallbacks *callbacks) {
  assert(ut_object_is_wayland_seat(object));
  UtWaylandSeat *self = (UtWaylandSeat *)object;

  uint32_t id = ut_wayland_client_allocate_id(self->client);
  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_uint(payload, id);
  ut_wayland_client_send_request(self->client, self->id, 1,
                                 ut_wayland_encoder_get_data(payload));
  return ut_wayland_keyboard_new(self->client, id, callback_object, callbacks);
}

UtObject *ut_wayland_seat_get_touch(UtObject *object, UtObject *callback_object,
                                    const UtWaylandTouchCallbacks *callbacks) {
  assert(ut_object_is_wayland_seat(object));
  UtWaylandSeat *self = (UtWaylandSeat *)object;

  uint32_t id = ut_wayland_client_allocate_id(self->client);
  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_uint(payload, id);
  ut_wayland_client_send_request(self->client, self->id, 2,
                                 ut_wayland_encoder_get_data(payload));
  return ut_wayland_touch_new(self->client, id, callback_object, callbacks);
}

void ut_wayland_seat_release(UtObject *object) {
  assert(ut_object_is_wayland_seat(object));
  UtWaylandSeat *self = (UtWaylandSeat *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_client_send_request(self->client, self->id, 3,
                                 ut_wayland_encoder_get_data(payload));
}

bool ut_object_is_wayland_seat(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
