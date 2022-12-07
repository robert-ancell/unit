#include <assert.h>

#include "ut-wayland-client-private.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *client;
  uint32_t id;
  const UtWaylandDisplayCallbacks *callbacks;
  void *user_data;
} UtWaylandDisplay;

static void decode_error(UtWaylandDisplay *self, UtObject *data) {
  uint32_t id = ut_wayland_decoder_get_uint(data);
  uint32_t code = ut_wayland_decoder_get_uint(data);
  ut_cstring_ref message = ut_wayland_decoder_get_string(data);
  if (self->callbacks != NULL && self->callbacks->error != NULL) {
    self->callbacks->error(self->user_data, id, code, message);
  }
}

static void decode_delete_id(UtWaylandDisplay *self, UtObject *data) {
  uint32_t id = ut_wayland_decoder_get_uint(data);
  if (self->callbacks != NULL && self->callbacks->delete_id != NULL) {
    self->callbacks->delete_id(self->user_data, id);
  }
}

static const char *ut_wayland_display_get_interface(UtObject *object) {
  return "wl_display";
}

static uint32_t ut_wayland_display_get_id(UtObject *object) {
  UtWaylandDisplay *self = (UtWaylandDisplay *)object;
  return self->id;
}

static bool ut_wayland_display_decode_event(UtObject *object, uint16_t code,
                                            UtObject *data) {
  UtWaylandDisplay *self = (UtWaylandDisplay *)object;
  switch (code) {
  case 0:
    decode_error(self, data);
    return true;
  case 1:
    decode_delete_id(self, data);
    return true;
  default:
    return false;
  }
}

static void ut_wayland_display_init(UtObject *object) {
  UtWaylandDisplay *self = (UtWaylandDisplay *)object;
  self->id = 1;
}

static UtWaylandObjectInterface wayland_object_interface = {
    .get_interface = ut_wayland_display_get_interface,
    .get_id = ut_wayland_display_get_id,
    .decode_event = ut_wayland_display_decode_event};

static UtObjectInterface object_interface = {
    .type_name = "UtWaylandDisplay",
    .init = ut_wayland_display_init,
    .interfaces = {{&ut_wayland_object_id, &wayland_object_interface},
                   {NULL, NULL}}};

UtObject *ut_wayland_display_new(UtObject *client,
                                 const UtWaylandDisplayCallbacks *callbacks,
                                 void *user_data) {
  UtObject *object = ut_object_new(sizeof(UtWaylandDisplay), &object_interface);
  UtWaylandDisplay *self = (UtWaylandDisplay *)object;
  self->client = client;
  self->callbacks = callbacks;
  self->user_data = user_data;
  ut_wayland_client_register_object(client, object);
  return object;
}

UtObject *ut_wayland_display_sync(UtObject *object,
                                  UtWaylandCallbackDoneCallback done_callback,
                                  void *user_data) {
  assert(ut_object_is_wayland_display(object));
  UtWaylandDisplay *self = (UtWaylandDisplay *)object;

  uint32_t callback = ut_wayland_client_allocate_id(self->client);
  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_uint(payload, callback);
  ut_wayland_client_send_request(self->client, self->id, 0,
                                 ut_wayland_encoder_get_data(payload));
  return ut_wayland_callback_new(self->client, callback, done_callback,
                                 user_data);
}

UtObject *
ut_wayland_display_get_registry(UtObject *object,
                                const UtWaylandRegistryCallbacks *callbacks,
                                void *user_data) {
  assert(ut_object_is_wayland_display(object));
  UtWaylandDisplay *self = (UtWaylandDisplay *)object;

  uint32_t id = ut_wayland_client_allocate_id(self->client);
  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_uint(payload, id);
  ut_wayland_client_send_request(self->client, self->id, 1,
                                 ut_wayland_encoder_get_data(payload));
  return ut_wayland_registry_new(self->client, id, callbacks, user_data);
  ;
}

bool ut_object_is_wayland_display(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
