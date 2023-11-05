#include <assert.h>

#include "ut-wayland-client-private.h"
#include "ut-wayland-registry-private.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *client;
  uint32_t id;
  UtObject *callback_object;
  const UtWaylandDrmCallbacks *callbacks;
  char *device;
  UtObject *formats;
  uint32_t capabilities;
} UtWaylandDrm;

static void decode_device(UtWaylandDrm *self, UtObject *data) {
  free(self->device);
  self->device = ut_wayland_decoder_get_string(data);
}

static void decode_format(UtWaylandDrm *self, UtObject *data) {
  uint32_t format = ut_wayland_decoder_get_uint(data);
  ut_uint32_list_append(self->formats, format);
}

static void decode_authenticated(UtWaylandDrm *self, UtObject *data) {
  if (self->callback_object != NULL && self->callbacks != NULL &&
      self->callbacks->authenticated != NULL) {
    self->callbacks->authenticated(self->callback_object);
  }
}

static void decode_capabilities(UtWaylandDrm *self, UtObject *data) {
  self->capabilities = ut_wayland_decoder_get_uint(data);
}

static const char *ut_wayland_drm_get_interface(UtObject *object) {
  return "wl_drm";
}

static uint32_t ut_wayland_drm_get_id(UtObject *object) {
  UtWaylandDrm *self = (UtWaylandDrm *)object;
  return self->id;
}

static bool ut_wayland_drm_decode_event(UtObject *object, uint16_t code,
                                        UtObject *data) {
  UtWaylandDrm *self = (UtWaylandDrm *)object;
  switch (code) {
  case 0:
    decode_device(self, data);
    return true;
  case 1:
    decode_format(self, data);
    return true;
  case 2:
    decode_authenticated(self, data);
    return true;
  case 3:
    decode_capabilities(self, data);
    return true;
  default:
    return false;
  }
}

static void ut_wayland_drm_init(UtObject *object) {
  UtWaylandDrm *self = (UtWaylandDrm *)object;
  self->device = ut_cstring_new("");
  self->formats = ut_uint32_list_new();
}

static void ut_wayland_drm_cleanup(UtObject *object) {
  UtWaylandDrm *self = (UtWaylandDrm *)object;
  ut_object_weak_unref(&self->callback_object);
  free(self->device);
  ut_object_unref(self->formats);
}

static UtWaylandObjectInterface wayland_object_interface = {
    .get_interface = ut_wayland_drm_get_interface,
    .get_id = ut_wayland_drm_get_id,
    .decode_event = ut_wayland_drm_decode_event};

static UtObjectInterface object_interface = {
    .type_name = "UtWaylandDrm",
    .init = ut_wayland_drm_init,
    .cleanup = ut_wayland_drm_cleanup,
    .interfaces = {{&ut_wayland_object_id, &wayland_object_interface},
                   {NULL, NULL}}};

UtObject *ut_wayland_drm_new(UtObject *client, uint32_t id,
                             UtObject *callback_object,
                             const UtWaylandDrmCallbacks *callbacks) {
  UtObject *object = ut_object_new(sizeof(UtWaylandDrm), &object_interface);
  UtWaylandDrm *self = (UtWaylandDrm *)object;
  self->client = client;
  self->id = id;
  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callbacks = callbacks;
  ut_wayland_client_register_object(client, object);
  return object;
}

UtObject *
ut_wayland_drm_new_from_registry(UtObject *registry, uint32_t name,
                                 UtObject *callback_object,
                                 const UtWaylandDrmCallbacks *callbacks) {
  uint32_t id = ut_wayland_registry_bind(registry, name, "wl_drm", 2);
  return ut_wayland_drm_new(ut_wayland_registry_get_client(registry), id,
                            callback_object, callbacks);
}

const char *ut_wayland_drm_get_device(UtObject *object) {
  assert(ut_object_is_wayland_drm(object));
  UtWaylandDrm *self = (UtWaylandDrm *)object;
  return self->device;
}

UtObject *ut_wayland_drm_get_formats(UtObject *object) {
  assert(ut_object_is_wayland_drm(object));
  UtWaylandDrm *self = (UtWaylandDrm *)object;
  return self->formats;
}

uint32_t ut_wayland_drm_get_capabilities(UtObject *object) {
  assert(ut_object_is_wayland_drm(object));
  UtWaylandDrm *self = (UtWaylandDrm *)object;
  return self->capabilities;
}

void ut_wayland_drm_authenticate(UtObject *object, uint32_t id) {
  assert(ut_object_is_wayland_drm(object));
  UtWaylandDrm *self = (UtWaylandDrm *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_uint(payload, id);
  ut_wayland_client_send_request(self->client, self->id, 0,
                                 ut_wayland_encoder_get_data(payload));
}

UtObject *
ut_wayland_drm_create_buffer(UtObject *object, uint32_t name, int32_t width,
                             int32_t height, uint32_t stride, uint32_t format,
                             UtObject *callback_object,
                             UtWaylandBufferReleaseCallback release_callback) {
  assert(ut_object_is_wayland_drm(object));
  UtWaylandDrm *self = (UtWaylandDrm *)object;

  uint32_t id = ut_wayland_client_allocate_id(self->client);
  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_uint(payload, id);
  ut_wayland_encoder_append_uint(payload, name);
  ut_wayland_encoder_append_int(payload, width);
  ut_wayland_encoder_append_int(payload, height);
  ut_wayland_encoder_append_uint(payload, stride);
  ut_wayland_encoder_append_uint(payload, format);
  ut_wayland_client_send_request(self->client, self->id, 1,
                                 ut_wayland_encoder_get_data(payload));
  UtObject *buffer = ut_wayland_buffer_new(self->client, id, callback_object,
                                           release_callback);
  ut_wayland_client_register_object(self->client, buffer);
  return buffer;
}

bool ut_object_is_wayland_drm(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
