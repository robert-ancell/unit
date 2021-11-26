#include <assert.h>

#include "ut-wayland-client-private.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *client;
  uint32_t id;
  const UtWaylandKeyboardCallbacks *callbacks;
  void *user_data;
} UtWaylandKeyboard;

static void decode_keymap(UtWaylandKeyboard *self, UtObject *data) {
  // FIXME
}

static void decode_enter(UtWaylandKeyboard *self, UtObject *data) {
  uint32_t serial = ut_wayland_decoder_get_uint(data);
  uint32_t surface_id = ut_wayland_decoder_get_uint(data);
  UtObject *surface = ut_wayland_client_get_object(self->client, surface_id);
  UtObject *keys = ut_wayland_decoder_get_uint_array(data);
  if (self->callbacks != NULL && self->callbacks->enter != NULL) {
    self->callbacks->enter(self->user_data, serial, surface, keys);
  }
}

static void decode_leave(UtWaylandKeyboard *self, UtObject *data) {
  uint32_t serial = ut_wayland_decoder_get_uint(data);
  uint32_t surface_id = ut_wayland_decoder_get_uint(data);
  UtObject *surface = ut_wayland_client_get_object(self->client, surface_id);
  if (self->callbacks != NULL && self->callbacks->leave != NULL) {
    self->callbacks->leave(self->user_data, serial, surface);
  }
}

static void decode_key(UtWaylandKeyboard *self, UtObject *data) {
  uint32_t serial = ut_wayland_decoder_get_uint(data);
  uint32_t time = ut_wayland_decoder_get_uint(data);
  uint32_t key = ut_wayland_decoder_get_uint(data);
  uint32_t state = ut_wayland_decoder_get_uint(data);
  if (self->callbacks != NULL && self->callbacks->key != NULL) {
    self->callbacks->key(self->user_data, serial, time, key, state);
  }
}

static void decode_modifiers(UtWaylandKeyboard *self, UtObject *data) {
  uint32_t serial = ut_wayland_decoder_get_uint(data);
  uint32_t mods_depressed = ut_wayland_decoder_get_uint(data);
  uint32_t mods_latched = ut_wayland_decoder_get_uint(data);
  uint32_t mods_locked = ut_wayland_decoder_get_uint(data);
  uint32_t group = ut_wayland_decoder_get_uint(data);
  if (self->callbacks != NULL && self->callbacks->modifiers != NULL) {
    self->callbacks->modifiers(self->user_data, serial, mods_depressed,
                               mods_latched, mods_locked, group);
  }
}

static void decode_repeat_info(UtWaylandKeyboard *self, UtObject *data) {
  uint32_t rate = ut_wayland_decoder_get_int(data);
  uint32_t delay = ut_wayland_decoder_get_int(data);
  if (self->callbacks != NULL && self->callbacks->repeat_info != NULL) {
    self->callbacks->repeat_info(self->user_data, rate, delay);
  }
}

static const char *ut_wayland_keyboard_get_interface(UtObject *object) {
  return "wl_keyboard";
}

static uint32_t ut_wayland_keyboard_get_id(UtObject *object) {
  UtWaylandKeyboard *self = (UtWaylandKeyboard *)object;
  return self->id;
}

static bool ut_wayland_keyboard_decode_event(UtObject *object, uint16_t code,
                                             UtObject *data) {
  UtWaylandKeyboard *self = (UtWaylandKeyboard *)object;
  switch (code) {
  case 0:
    decode_keymap(self, data);
    return true;
  case 1:
    decode_enter(self, data);
    return true;
  case 2:
    decode_leave(self, data);
    return true;
  case 3:
    decode_key(self, data);
    return true;
  case 4:
    decode_modifiers(self, data);
    return true;
  case 5:
    decode_repeat_info(self, data);
    return true;
  default:
    return false;
  }
}

static UtWaylandObjectInterface wayland_object_interface = {
    .get_interface = ut_wayland_keyboard_get_interface,
    .get_id = ut_wayland_keyboard_get_id,
    .decode_event = ut_wayland_keyboard_decode_event};

static UtObjectInterface object_interface = {
    .type_name = "UtWaylandKeyboard",
    .interfaces = {{&ut_wayland_object_id, &wayland_object_interface},
                   {NULL, NULL}}};

UtObject *ut_wayland_keyboard_new(UtObject *client, uint32_t id,
                                  const UtWaylandKeyboardCallbacks *callbacks,
                                  void *user_data) {
  UtObject *object =
      ut_object_new(sizeof(UtWaylandKeyboard), &object_interface);
  UtWaylandKeyboard *self = (UtWaylandKeyboard *)object;
  self->client = client;
  self->id = id;
  self->callbacks = callbacks;
  self->user_data = user_data;
  ut_wayland_client_register_object(client, object);
  return object;
}

void ut_wayland_keyboard_release(UtObject *object) {
  assert(ut_object_is_wayland_keyboard(object));
  UtWaylandKeyboard *self = (UtWaylandKeyboard *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_client_send_request(self->client, self->id, 0,
                                 ut_wayland_encoder_get_data(payload));
}

bool ut_object_is_wayland_keyboard(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
