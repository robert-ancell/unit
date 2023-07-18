#include <assert.h>

#include "ut-wayland-client-private.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *client;
  uint32_t id;
  UtObject *callback_object;
  const UtXdgPopupCallbacks *callbacks;
} UtXdgPopup;

static void decode_configure(UtXdgPopup *self, UtObject *data) {
  int32_t x = ut_wayland_decoder_get_int(data);
  int32_t y = ut_wayland_decoder_get_int(data);
  int32_t width = ut_wayland_decoder_get_int(data);
  int32_t height = ut_wayland_decoder_get_int(data);
  if (self->callbacks != NULL && self->callbacks->configure != NULL) {
    self->callbacks->configure(self->callback_object, x, y, width, height);
  }
}

static void decode_popup_done(UtXdgPopup *self, UtObject *data) {
  if (self->callbacks != NULL && self->callbacks->popup_done != NULL) {
    self->callbacks->popup_done(self->callback_object);
  }
}

static void decode_repositioned(UtXdgPopup *self, UtObject *data) {
  uint32_t token = ut_wayland_decoder_get_uint(data);
  if (self->callbacks != NULL && self->callbacks->repositioned != NULL) {
    self->callbacks->repositioned(self->callback_object, token);
  }
}

static const char *ut_xdg_popup_get_interface(UtObject *object) {
  return "xdg_popup";
}

static uint32_t ut_xdg_popup_get_id(UtObject *object) {
  UtXdgPopup *self = (UtXdgPopup *)object;
  return self->id;
}

static bool ut_xdg_popup_decode_event(UtObject *object, uint16_t code,
                                      UtObject *data) {
  UtXdgPopup *self = (UtXdgPopup *)object;
  switch (code) {
  case 0:
    decode_configure(self, data);
    return true;
  case 1:
    decode_popup_done(self, data);
    return true;
  case 2:
    decode_repositioned(self, data);
    return true;
  default:
    return false;
  }
}

static void ut_xdg_popup_cleanup(UtObject *object) {
  UtXdgPopup *self = (UtXdgPopup *)object;
  ut_object_weak_unref(&self->callback_object);
}

static UtWaylandObjectInterface wayland_object_interface = {
    .get_interface = ut_xdg_popup_get_interface,
    .get_id = ut_xdg_popup_get_id,
    .decode_event = ut_xdg_popup_decode_event};

static UtObjectInterface object_interface = {
    .type_name = "UtXdgPopup",
    .cleanup = ut_xdg_popup_cleanup,
    .interfaces = {{&ut_wayland_object_id, &wayland_object_interface},
                   {NULL, NULL}}};

UtObject *ut_xdg_popup_new(UtObject *client, uint32_t id,
                           UtObject *callback_object,
                           const UtXdgPopupCallbacks *callbacks) {
  UtObject *object = ut_object_new(sizeof(UtXdgPopup), &object_interface);
  UtXdgPopup *self = (UtXdgPopup *)object;
  self->client = client;
  self->id = id;
  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callbacks = callbacks;
  ut_wayland_client_register_object(client, object);
  return object;
}

void ut_xdg_popup_destroy(UtObject *object) {
  assert(ut_object_is_xdg_popup(object));
  UtXdgPopup *self = (UtXdgPopup *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_client_send_request(self->client, self->id, 0,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_xdg_popup_grab(UtObject *object, UtObject *seat, uint32_t serial) {
  assert(ut_object_is_xdg_popup(object));
  UtXdgPopup *self = (UtXdgPopup *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_object(payload, seat);
  ut_wayland_encoder_append_uint(payload, serial);
  ut_wayland_client_send_request(self->client, self->id, 0,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_xdg_popup_reposition(UtObject *object, UtObject *positioner,
                             uint32_t token) {
  assert(ut_object_is_xdg_popup(object));
  UtXdgPopup *self = (UtXdgPopup *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_object(payload, positioner);
  ut_wayland_encoder_append_uint(payload, token);
  ut_wayland_client_send_request(self->client, self->id, 0,
                                 ut_wayland_encoder_get_data(payload));
}

bool ut_object_is_xdg_popup(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
