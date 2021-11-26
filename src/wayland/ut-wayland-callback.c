#include <assert.h>

#include "ut-wayland-client-private.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *client;
  uint32_t id;
  UtWaylandCallbackDoneCallback done_callback;
  void *user_data;
} UtWaylandCallback;

static void decode_done(UtWaylandCallback *self, UtObject *data) {
  uint32_t callback_data = ut_wayland_decoder_get_uint(data);
  if (self->done_callback != NULL) {
    self->done_callback(self->user_data, callback_data);
  }
}

static const char *ut_wayland_callback_get_interface(UtObject *object) {
  return "wl_callback";
}

static uint32_t ut_wayland_callback_get_id(UtObject *object) {
  UtWaylandCallback *self = (UtWaylandCallback *)object;
  return self->id;
}

static bool ut_wayland_callback_decode_event(UtObject *object, uint16_t code,
                                             UtObject *data) {
  UtWaylandCallback *self = (UtWaylandCallback *)object;
  switch (code) {
  case 0:
    decode_done(self, data);
    return true;
  default:
    return false;
  }
}

static UtWaylandObjectInterface wayland_object_interface = {
    .get_interface = ut_wayland_callback_get_interface,
    .get_id = ut_wayland_callback_get_id,
    .decode_event = ut_wayland_callback_decode_event};

static UtObjectInterface object_interface = {
    .type_name = "UtWaylandCallback",
    .interfaces = {{&ut_wayland_object_id, &wayland_object_interface},
                   {NULL, NULL}}};

UtObject *ut_wayland_callback_new(UtObject *client, uint32_t id,
                                  UtWaylandCallbackDoneCallback done_callback,
                                  void *user_data) {
  UtObject *object =
      ut_object_new(sizeof(UtWaylandCallback), &object_interface);
  UtWaylandCallback *self = (UtWaylandCallback *)object;
  self->client = client;
  self->id = id;
  self->done_callback = done_callback;
  self->user_data = user_data;
  ut_wayland_client_register_object(client, object);
  return object;
}

bool ut_object_is_wayland_callback(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
