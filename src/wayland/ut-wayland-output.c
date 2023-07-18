#include <assert.h>

#include "ut-wayland-client-private.h"
#include "ut-wayland-registry-private.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *client;
  uint32_t id;
  UtObject *callback_object;
  const UtWaylandOutputCallbacks *callbacks;
} UtWaylandOutput;

static void decode_geometry(UtWaylandOutput *self, UtObject *data) {
  int32_t x = ut_wayland_decoder_get_int(data);
  int32_t y = ut_wayland_decoder_get_int(data);
  int32_t physical_width = ut_wayland_decoder_get_int(data);
  int32_t physical_height = ut_wayland_decoder_get_int(data);
  int32_t subpixel = ut_wayland_decoder_get_int(data); // FIXME: enum
  ut_cstring_ref make = ut_wayland_decoder_get_string(data);
  ut_cstring_ref model = ut_wayland_decoder_get_string(data);
  int32_t transform = ut_wayland_decoder_get_int(data); // FIXME: enum
  if (self->callback_object != NULL && self->callbacks != NULL &&
      self->callbacks->geometry != NULL) {
    self->callbacks->geometry(self->callback_object, x, y, physical_width,
                              physical_height, subpixel, make, model,
                              transform);
  }
}

static void decode_mode(UtWaylandOutput *self, UtObject *data) {
  uint32_t flags = ut_wayland_decoder_get_uint(data);
  int32_t width = ut_wayland_decoder_get_int(data);
  int32_t height = ut_wayland_decoder_get_int(data);
  int32_t refresh = ut_wayland_decoder_get_int(data);
  if (self->callback_object != NULL && self->callbacks != NULL &&
      self->callbacks->mode != NULL) {
    self->callbacks->mode(self->callback_object, flags, width, height, refresh);
  }
}

static void decode_done(UtWaylandOutput *self, UtObject *data) {
  if (self->callback_object != NULL && self->callbacks != NULL &&
      self->callbacks->done != NULL) {
    self->callbacks->done(self->callback_object);
  }
}

static void decode_scale(UtWaylandOutput *self, UtObject *data) {
  int32_t scale = ut_wayland_decoder_get_int(data);
  if (self->callback_object != NULL && self->callbacks != NULL &&
      self->callbacks->scale != NULL) {
    self->callbacks->scale(self->callback_object, scale);
  }
}

static void decode_name(UtWaylandOutput *self, UtObject *data) {
  ut_cstring_ref name = ut_wayland_decoder_get_string(data);
  if (self->callback_object != NULL && self->callbacks != NULL &&
      self->callbacks->name != NULL) {
    self->callbacks->name(self->callback_object, name);
  }
}

static void decode_description(UtWaylandOutput *self, UtObject *data) {
  ut_cstring_ref description = ut_wayland_decoder_get_string(data);
  if (self->callback_object != NULL && self->callbacks != NULL &&
      self->callbacks->description != NULL) {
    self->callbacks->description(self->callback_object, description);
  }
}

static const char *ut_wayland_output_get_interface(UtObject *object) {
  return "wl_output";
}

static uint32_t ut_wayland_output_get_id(UtObject *object) {
  UtWaylandOutput *self = (UtWaylandOutput *)object;
  return self->id;
}

static bool ut_wayland_output_decode_event(UtObject *object, uint16_t code,
                                           UtObject *data) {
  UtWaylandOutput *self = (UtWaylandOutput *)object;
  switch (code) {
  case 0:
    decode_geometry(self, data);
    return true;
  case 1:
    decode_mode(self, data);
    return true;
  case 2:
    decode_done(self, data);
    return true;
  case 3:
    decode_scale(self, data);
    return true;
  case 4:
    decode_name(self, data);
    return true;
  case 5:
    decode_description(self, data);
    return true;
  default:
    return false;
  }
}

static void ut_wayland_output_cleanup(UtObject *object) {
  UtWaylandOutput *self = (UtWaylandOutput *)object;
  ut_object_weak_unref(&self->callback_object);
}

static UtWaylandObjectInterface wayland_object_interface = {
    .get_interface = ut_wayland_output_get_interface,
    .get_id = ut_wayland_output_get_id,
    .decode_event = ut_wayland_output_decode_event};

static UtObjectInterface object_interface = {
    .type_name = "UtWaylandOutput",
    .cleanup = ut_wayland_output_cleanup,
    .interfaces = {{&ut_wayland_object_id, &wayland_object_interface},
                   {NULL, NULL}}};

UtObject *ut_wayland_output_new(UtObject *client, uint32_t id,
                                UtObject *callback_object,
                                const UtWaylandOutputCallbacks *callbacks) {
  UtObject *object = ut_object_new(sizeof(UtWaylandOutput), &object_interface);
  UtWaylandOutput *self = (UtWaylandOutput *)object;
  self->client = client;
  self->id = id;
  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callbacks = callbacks;
  ut_wayland_client_register_object(client, object);
  return object;
}

UtObject *
ut_wayland_output_new_from_registry(UtObject *registry, uint32_t name,
                                    UtObject *callback_object,
                                    const UtWaylandOutputCallbacks *callbacks) {
  uint32_t id = ut_wayland_registry_bind(registry, name, "wl_output", 2);
  return ut_wayland_output_new(ut_wayland_registry_get_client(registry), id,
                               callback_object, callbacks);
}

void ut_wayland_output_release(UtObject *object) {
  assert(ut_object_is_wayland_output(object));
  UtWaylandOutput *self = (UtWaylandOutput *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_client_send_request(self->client, self->id, 0,
                                 ut_wayland_encoder_get_data(payload));
}

bool ut_object_is_wayland_output(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
