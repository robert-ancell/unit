#include <assert.h>

#include "ut-wayland-client-private.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *client;
  uint32_t id;
  UtObject *callback_object;
  const UtXdgToplevelCallbacks *callbacks;
} UtXdgToplevel;

static void decode_configure(UtXdgToplevel *self, UtObject *data) {
  int32_t width = ut_wayland_decoder_get_int(data);
  int32_t height = ut_wayland_decoder_get_int(data);
  UtObjectRef states = ut_wayland_decoder_get_uint_array(data);
  if (self->callbacks != NULL && self->callbacks->configure != NULL) {
    self->callbacks->configure(self->callback_object, width, height, states);
  }
}

static void decode_close(UtXdgToplevel *self, UtObject *data) {
  if (self->callbacks != NULL && self->callbacks->close != NULL) {
    self->callbacks->close(self->callback_object);
  }
}

static void decode_configure_bounds(UtXdgToplevel *self, UtObject *data) {
  int32_t width = ut_wayland_decoder_get_int(data);
  int32_t height = ut_wayland_decoder_get_int(data);
  if (self->callbacks != NULL && self->callbacks->configure_bounds != NULL) {
    self->callbacks->configure_bounds(self->callback_object, width, height);
  }
}

static void decode_wm_capabilities(UtXdgToplevel *self, UtObject *data) {
  UtObjectRef capabilities = ut_wayland_decoder_get_uint_array(data);
  if (self->callbacks != NULL && self->callbacks->wm_capabilities != NULL) {
    self->callbacks->wm_capabilities(self->callback_object, capabilities);
  }
}

static const char *ut_xdg_toplevel_get_interface(UtObject *object) {
  return "xdg_toplevel";
}

static uint32_t ut_xdg_toplevel_get_id(UtObject *object) {
  UtXdgToplevel *self = (UtXdgToplevel *)object;
  return self->id;
}

static bool ut_xdg_toplevel_decode_event(UtObject *object, uint16_t code,
                                         UtObject *data) {
  UtXdgToplevel *self = (UtXdgToplevel *)object;
  switch (code) {
  case 0:
    decode_configure(self, data);
    return true;
  case 1:
    decode_close(self, data);
    return true;
  case 2:
    decode_configure_bounds(self, data);
    return true;
  case 3:
    decode_wm_capabilities(self, data);
    return true;
  default:
    return false;
  }
}

static void ut_xdg_toplevel_cleanup(UtObject *object) {
  UtXdgToplevel *self = (UtXdgToplevel *)object;
  ut_object_weak_unref(&self->callback_object);
}

static UtWaylandObjectInterface wayland_object_interface = {
    .get_interface = ut_xdg_toplevel_get_interface,
    .get_id = ut_xdg_toplevel_get_id,
    .decode_event = ut_xdg_toplevel_decode_event};

static UtObjectInterface object_interface = {
    .type_name = "UtXdgToplevel",
    .cleanup = ut_xdg_toplevel_cleanup,
    .interfaces = {{&ut_wayland_object_id, &wayland_object_interface},
                   {NULL, NULL}}};

UtObject *ut_xdg_toplevel_new(UtObject *client, uint32_t id,
                              UtObject *callback_object,
                              const UtXdgToplevelCallbacks *callbacks) {
  UtObject *object = ut_object_new(sizeof(UtXdgToplevel), &object_interface);
  UtXdgToplevel *self = (UtXdgToplevel *)object;
  self->client = client;
  self->id = id;
  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callbacks = callbacks;
  ut_wayland_client_register_object(client, object);
  return object;
}

void ut_xdg_toplevel_destroy(UtObject *object) {
  assert(ut_object_is_xdg_toplevel(object));
  UtXdgToplevel *self = (UtXdgToplevel *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_client_send_request(self->client, self->id, 0,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_xdg_toplevel_set_parent(UtObject *object, UtObject *parent) {
  assert(ut_object_is_xdg_toplevel(object));
  UtXdgToplevel *self = (UtXdgToplevel *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_object(payload, parent);
  ut_wayland_client_send_request(self->client, self->id, 1,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_xdg_toplevel_set_title(UtObject *object, const char *title) {
  assert(ut_object_is_xdg_toplevel(object));
  UtXdgToplevel *self = (UtXdgToplevel *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_string(payload, title);
  ut_wayland_client_send_request(self->client, self->id, 2,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_xdg_toplevel_set_app_id(UtObject *object, const char *app_id) {
  assert(ut_object_is_xdg_toplevel(object));
  UtXdgToplevel *self = (UtXdgToplevel *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_string(payload, app_id);
  ut_wayland_client_send_request(self->client, self->id, 3,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_xdg_toplevel_show_window_menu(UtObject *object, UtObject *seat,
                                      uint32_t serial, int32_t x, int32_t y) {
  assert(ut_object_is_xdg_toplevel(object));
  UtXdgToplevel *self = (UtXdgToplevel *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_object(payload, seat);
  ut_wayland_encoder_append_uint(payload, serial);
  ut_wayland_encoder_append_int(payload, x);
  ut_wayland_encoder_append_int(payload, y);
  ut_wayland_client_send_request(self->client, self->id, 4,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_xdg_toplevel_move(UtObject *object, UtObject *seat, uint32_t serial) {
  assert(ut_object_is_xdg_toplevel(object));
  UtXdgToplevel *self = (UtXdgToplevel *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_object(payload, seat);
  ut_wayland_encoder_append_uint(payload, serial);
  ut_wayland_client_send_request(self->client, self->id, 5,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_xdg_toplevel_resize(UtObject *object, UtObject *seat, uint32_t serial,
                            uint32_t edge) {
  assert(ut_object_is_xdg_toplevel(object));
  UtXdgToplevel *self = (UtXdgToplevel *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_object(payload, seat);
  ut_wayland_encoder_append_uint(payload, serial);
  ut_wayland_encoder_append_uint(payload, edge);
  ut_wayland_client_send_request(self->client, self->id, 6,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_xdg_toplevel_set_max_size(UtObject *object, int32_t width,
                                  int32_t height) {
  assert(ut_object_is_xdg_toplevel(object));
  UtXdgToplevel *self = (UtXdgToplevel *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_int(payload, width);
  ut_wayland_encoder_append_int(payload, height);
  ut_wayland_client_send_request(self->client, self->id, 7,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_xdg_toplevel_set_min_size(UtObject *object, int32_t width,
                                  int32_t height) {
  assert(ut_object_is_xdg_toplevel(object));
  UtXdgToplevel *self = (UtXdgToplevel *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_int(payload, width);
  ut_wayland_encoder_append_int(payload, height);
  ut_wayland_client_send_request(self->client, self->id, 8,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_xdg_toplevel_set_maximized(UtObject *object) {
  assert(ut_object_is_xdg_toplevel(object));
  UtXdgToplevel *self = (UtXdgToplevel *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_client_send_request(self->client, self->id, 9,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_xdg_toplevel_unset_maximized(UtObject *object) {
  assert(ut_object_is_xdg_toplevel(object));
  UtXdgToplevel *self = (UtXdgToplevel *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_client_send_request(self->client, self->id, 10,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_xdg_toplevel_set_fullscreen(UtObject *object, UtObject *output) {
  assert(ut_object_is_xdg_toplevel(object));
  UtXdgToplevel *self = (UtXdgToplevel *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_object(payload, output);
  ut_wayland_client_send_request(self->client, self->id, 11,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_xdg_toplevel_unset_fullscreen(UtObject *object) {
  assert(ut_object_is_xdg_toplevel(object));
  UtXdgToplevel *self = (UtXdgToplevel *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_client_send_request(self->client, self->id, 12,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_xdg_toplevel_set_minimized(UtObject *object) {
  assert(ut_object_is_xdg_toplevel(object));
  UtXdgToplevel *self = (UtXdgToplevel *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_client_send_request(self->client, self->id, 13,
                                 ut_wayland_encoder_get_data(payload));
}

bool ut_object_is_xdg_toplevel(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
