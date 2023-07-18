#include <assert.h>

#include "ut-wayland-client-private.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *client;
  uint32_t id;
  UtObject *callback_object;
  const UtWaylandPointerCallbacks *callbacks;
} UtWaylandPointer;

static void decode_enter(UtWaylandPointer *self, UtObject *data) {
  uint32_t serial = ut_wayland_decoder_get_uint(data);
  uint32_t surface_id = ut_wayland_decoder_get_uint(data);
  UtObject *surface = ut_wayland_client_get_object(self->client, surface_id);
  double surface_x = ut_wayland_decoder_get_fixed(data);
  double surface_y = ut_wayland_decoder_get_fixed(data);
  if (self->callback_object != NULL && self->callbacks != NULL &&
      self->callbacks->enter != NULL) {
    self->callbacks->enter(self->callback_object, serial, surface, surface_x,
                           surface_y);
  }
}

static void decode_leave(UtWaylandPointer *self, UtObject *data) {
  uint32_t serial = ut_wayland_decoder_get_uint(data);
  uint32_t surface_id = ut_wayland_decoder_get_uint(data);
  UtObject *surface = ut_wayland_client_get_object(self->client, surface_id);
  if (self->callback_object != NULL && self->callbacks != NULL &&
      self->callbacks->leave != NULL) {
    self->callbacks->leave(self->callback_object, serial, surface);
  }
}

static void decode_motion(UtWaylandPointer *self, UtObject *data) {
  uint32_t time = ut_wayland_decoder_get_uint(data);
  double surface_x = ut_wayland_decoder_get_fixed(data);
  double surface_y = ut_wayland_decoder_get_fixed(data);
  if (self->callback_object != NULL && self->callbacks != NULL &&
      self->callbacks->motion != NULL) {
    self->callbacks->motion(self->callback_object, time, surface_x, surface_y);
  }
}

static void decode_button(UtWaylandPointer *self, UtObject *data) {
  uint32_t serial = ut_wayland_decoder_get_uint(data);
  uint32_t time = ut_wayland_decoder_get_uint(data);
  uint32_t button = ut_wayland_decoder_get_uint(data);
  uint32_t state = ut_wayland_decoder_get_uint(data);
  if (self->callback_object != NULL && self->callbacks != NULL &&
      self->callbacks->button != NULL) {
    self->callbacks->button(self->callback_object, serial, time, button, state);
  }
}

static void decode_axis(UtWaylandPointer *self, UtObject *data) {
  // FIXME
}

static void decode_frame(UtWaylandPointer *self, UtObject *data) {
  if (self->callback_object != NULL && self->callbacks != NULL &&
      self->callbacks->frame != NULL) {
    self->callbacks->frame(self->callback_object);
  }
}

static void decode_axis_source(UtWaylandPointer *self, UtObject *data) {
  // FIXME
}

static void decode_axis_stop(UtWaylandPointer *self, UtObject *data) {
  // FIXME
}

static void decode_axis_discrete(UtWaylandPointer *self, UtObject *data) {
  // FIXME
}

static void decode_axis_value120(UtWaylandPointer *self, UtObject *data) {
  // FIXME
}

static const char *ut_wayland_pointer_get_interface(UtObject *object) {
  return "wl_pointer";
}

static uint32_t ut_wayland_pointer_get_id(UtObject *object) {
  UtWaylandPointer *self = (UtWaylandPointer *)object;
  return self->id;
}

static bool ut_wayland_pointer_decode_event(UtObject *object, uint16_t code,
                                            UtObject *data) {
  UtWaylandPointer *self = (UtWaylandPointer *)object;
  switch (code) {
  case 0:
    decode_enter(self, data);
    return true;
  case 1:
    decode_leave(self, data);
    return true;
  case 2:
    decode_motion(self, data);
    return true;
  case 3:
    decode_button(self, data);
    return true;
  case 4:
    decode_axis(self, data);
    return true;
  case 5:
    decode_frame(self, data);
    return true;
  case 6:
    decode_axis_source(self, data);
    return true;
  case 7:
    decode_axis_stop(self, data);
    return true;
  case 8:
    decode_axis_discrete(self, data);
    return true;
  case 9:
    decode_axis_value120(self, data);
    return true;
  default:
    return false;
  }
}

static void ut_wayland_pointer_cleanup(UtObject *object) {
  UtWaylandPointer *self = (UtWaylandPointer *)object;
  ut_object_weak_unref(&self->callback_object);
}

static UtWaylandObjectInterface wayland_object_interface = {
    .get_interface = ut_wayland_pointer_get_interface,
    .get_id = ut_wayland_pointer_get_id,
    .decode_event = ut_wayland_pointer_decode_event};

static UtObjectInterface object_interface = {
    .type_name = "UtWaylandPointer",
    .cleanup = ut_wayland_pointer_cleanup,
    .interfaces = {{&ut_wayland_object_id, &wayland_object_interface},
                   {NULL, NULL}}};

UtObject *ut_wayland_pointer_new(UtObject *client, uint32_t id,
                                 UtObject *callback_object,
                                 const UtWaylandPointerCallbacks *callbacks) {
  UtObject *object = ut_object_new(sizeof(UtWaylandPointer), &object_interface);
  UtWaylandPointer *self = (UtWaylandPointer *)object;
  self->client = client;
  self->id = id;
  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callbacks = callbacks;
  ut_wayland_client_register_object(client, object);
  return object;
}

void ut_wayland_pointer_set_cursor(UtObject *object, uint32_t serial,
                                   UtObject *surface, int32_t hotspot_x,
                                   int32_t hotspot_y) {
  assert(ut_object_is_wayland_pointer(object));
  UtWaylandPointer *self = (UtWaylandPointer *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_uint(payload, serial);
  ut_wayland_encoder_append_object(payload, surface);
  ut_wayland_encoder_append_int(payload, hotspot_x);
  ut_wayland_encoder_append_int(payload, hotspot_y);
  ut_wayland_client_send_request(self->client, self->id, 0,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_wayland_pointer_release(UtObject *object) {
  assert(ut_object_is_wayland_pointer(object));
  UtWaylandPointer *self = (UtWaylandPointer *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_client_send_request(self->client, self->id, 1,
                                 ut_wayland_encoder_get_data(payload));
}

bool ut_object_is_wayland_pointer(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
