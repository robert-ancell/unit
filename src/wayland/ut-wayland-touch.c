#include <assert.h>

#include "ut-wayland-client-private.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *client;
  uint32_t id;
  const UtWaylandTouchCallbacks *callbacks;
  void *user_data;
} UtWaylandTouch;

static void decode_down(UtWaylandTouch *self, UtObject *data) {
  uint32_t serial = ut_wayland_decoder_get_uint(data);
  uint32_t time = ut_wayland_decoder_get_uint(data);
  uint32_t surface_id = ut_wayland_decoder_get_uint(data);
  UtObject *surface = ut_wayland_client_get_object(self->client, surface_id);
  int32_t id = ut_wayland_decoder_get_int(data);
  double surface_x = ut_wayland_decoder_get_fixed(data);
  double surface_y = ut_wayland_decoder_get_fixed(data);
  if (self->callbacks != NULL && self->callbacks->down != NULL) {
    self->callbacks->down(self->user_data, serial, time, surface, id, surface_x,
                          surface_y);
  }
}

static void decode_up(UtWaylandTouch *self, UtObject *data) {
  uint32_t serial = ut_wayland_decoder_get_uint(data);
  uint32_t time = ut_wayland_decoder_get_uint(data);
  int32_t id = ut_wayland_decoder_get_int(data);
  if (self->callbacks != NULL && self->callbacks->up != NULL) {
    self->callbacks->up(self->user_data, serial, time, id);
  }
}

static void decode_motion(UtWaylandTouch *self, UtObject *data) {
  uint32_t time = ut_wayland_decoder_get_uint(data);
  int32_t id = ut_wayland_decoder_get_int(data);
  double surface_x = ut_wayland_decoder_get_fixed(data);
  double surface_y = ut_wayland_decoder_get_fixed(data);
  if (self->callbacks != NULL && self->callbacks->motion != NULL) {
    self->callbacks->motion(self->user_data, time, id, surface_x, surface_y);
  }
}

static void decode_frame(UtWaylandTouch *self, UtObject *data) {
  if (self->callbacks != NULL && self->callbacks->frame != NULL) {
    self->callbacks->frame(self->user_data);
  }
}

static void decode_cancel(UtWaylandTouch *self, UtObject *data) {
  if (self->callbacks != NULL && self->callbacks->cancel != NULL) {
    self->callbacks->cancel(self->user_data);
  }
}

static void decode_shape(UtWaylandTouch *self, UtObject *data) {
  int32_t id = ut_wayland_decoder_get_int(data);
  double major = ut_wayland_decoder_get_fixed(data);
  double minor = ut_wayland_decoder_get_fixed(data);
  if (self->callbacks != NULL && self->callbacks->shape != NULL) {
    self->callbacks->shape(self->user_data, id, major, minor);
  }
}

static void decode_orientation(UtWaylandTouch *self, UtObject *data) {
  int32_t id = ut_wayland_decoder_get_int(data);
  double orientation = ut_wayland_decoder_get_fixed(data);
  if (self->callbacks != NULL && self->callbacks->orientation != NULL) {
    self->callbacks->orientation(self->user_data, id, orientation);
  }
}

static const char *ut_wayland_touch_get_interface(UtObject *object) {
  return "wl_touch";
}

static uint32_t ut_wayland_touch_get_id(UtObject *object) {
  UtWaylandTouch *self = (UtWaylandTouch *)object;
  return self->id;
}

static bool ut_wayland_touch_decode_event(UtObject *object, uint16_t code,
                                          UtObject *data) {
  UtWaylandTouch *self = (UtWaylandTouch *)object;
  switch (code) {
  case 0:
    decode_down(self, data);
    return true;
  case 1:
    decode_up(self, data);
    return true;
  case 2:
    decode_motion(self, data);
    return true;
  case 3:
    decode_frame(self, data);
    return true;
  case 4:
    decode_cancel(self, data);
    return true;
  case 5:
    decode_shape(self, data);
    return true;
  case 6:
    decode_orientation(self, data);
    return true;
  default:
    return false;
  }
}

static UtWaylandObjectInterface wayland_object_interface = {
    .get_interface = ut_wayland_touch_get_interface,
    .get_id = ut_wayland_touch_get_id,
    .decode_event = ut_wayland_touch_decode_event};

static UtObjectInterface object_interface = {
    .type_name = "UtWaylandTouch",
    .interfaces = {{&ut_wayland_object_id, &wayland_object_interface},
                   {NULL, NULL}}};

UtObject *ut_wayland_touch_new(UtObject *client, uint32_t id,
                               const UtWaylandTouchCallbacks *callbacks,
                               void *user_data) {
  UtObject *object = ut_object_new(sizeof(UtWaylandTouch), &object_interface);
  UtWaylandTouch *self = (UtWaylandTouch *)object;
  self->client = client;
  self->id = id;
  self->callbacks = callbacks;
  self->user_data = user_data;
  ut_wayland_client_register_object(client, object);
  return object;
}

void ut_wayland_touch_release(UtObject *object) {
  assert(ut_object_is_wayland_touch(object));
  UtWaylandTouch *self = (UtWaylandTouch *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_client_send_request(self->client, self->id, 0,
                                 ut_wayland_encoder_get_data(payload));
}

bool ut_object_is_wayland_touch(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
