#include <assert.h>

#include "ut-wayland-client-private.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *client;
  uint32_t id;
  UtWaylandBufferReleaseCallback release_callback;
  void *user_data;
} UtWaylandBuffer;

static void decode_release(UtWaylandBuffer *self, UtObject *data) {
  if (self->release_callback != NULL) {
    self->release_callback(self->user_data);
  }
}

static const char *ut_wayland_buffer_get_interface(UtObject *object) {
  return "wl_buffer";
}

static uint32_t ut_wayland_buffer_get_id(UtObject *object) {
  UtWaylandBuffer *self = (UtWaylandBuffer *)object;
  return self->id;
}

static bool ut_wayland_buffer_decode_event(UtObject *object, uint16_t code,
                                           UtObject *data) {
  UtWaylandBuffer *self = (UtWaylandBuffer *)object;
  switch (code) {
  case 0:
    decode_release(self, data);
    return true;
  default:
    return false;
  }
}

static UtWaylandObjectInterface wayland_object_interface = {
    .get_interface = ut_wayland_buffer_get_interface,
    .get_id = ut_wayland_buffer_get_id,
    .decode_event = ut_wayland_buffer_decode_event};

static UtObjectInterface object_interface = {
    .type_name = "UtWaylandBuffer",
    .interfaces = {{&ut_wayland_object_id, &wayland_object_interface},
                   {NULL, NULL}}};

UtObject *ut_wayland_buffer_new(UtObject *client, uint32_t id,
                                UtWaylandBufferReleaseCallback release_callback,
                                void *user_data) {
  UtObject *object = ut_object_new(sizeof(UtWaylandBuffer), &object_interface);
  UtWaylandBuffer *self = (UtWaylandBuffer *)object;
  self->client = client;
  self->id = id;
  self->release_callback = release_callback;
  self->user_data = user_data;
  ut_wayland_client_register_object(client, object);
  return object;
}

void ut_wayland_buffer_destroy(UtObject *object) {
  assert(ut_object_is_wayland_buffer(object));
  UtWaylandBuffer *self = (UtWaylandBuffer *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_client_send_request(self->client, self->id, 0,
                                 ut_wayland_encoder_get_data(payload));
}

bool ut_object_is_wayland_buffer(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
