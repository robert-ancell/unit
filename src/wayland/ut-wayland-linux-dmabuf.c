#include <assert.h>

#include "ut-wayland-client-private.h"
#include "ut-wayland-registry-private.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *client;
  uint32_t id;
  UtObject *formats;
  UtObject *modifiers;
} UtWaylandLinuxDmabuf;

static void decode_format(UtWaylandLinuxDmabuf *self, UtObject *data) {
  uint32_t format = ut_wayland_decoder_get_uint(data);
  ut_uint32_list_append(self->formats, format);
}

static void decode_modifier(UtWaylandLinuxDmabuf *self, UtObject *data) {
  // FIXMEuint32_t modifier = ut_wayland_decoder_get_uint(data);
  // ut_list_append(self->modifiers, modifier);
}

static const char *ut_wayland_linux_dmabuf_get_interface(UtObject *object) {
  return "zwp_linux_dmabuf_v1";
}

static uint32_t ut_wayland_linux_dmabuf_get_id(UtObject *object) {
  UtWaylandLinuxDmabuf *self = (UtWaylandLinuxDmabuf *)object;
  return self->id;
}

static bool ut_wayland_linux_dmabuf_decode_event(UtObject *object,
                                                 uint16_t code,
                                                 UtObject *data) {
  UtWaylandLinuxDmabuf *self = (UtWaylandLinuxDmabuf *)object;
  switch (code) {
  case 0:
    decode_format(self, data);
    return true;
  case 1:
    decode_modifier(self, data);
    return true;
  default:
    return false;
  }
}

static void ut_wayland_linux_dmabuf_init(UtObject *object) {
  UtWaylandLinuxDmabuf *self = (UtWaylandLinuxDmabuf *)object;
  self->formats = ut_uint32_list_new();
  self->modifiers = ut_list_new();
}

static void ut_wayland_linux_dmabuf_cleanup(UtObject *object) {
  UtWaylandLinuxDmabuf *self = (UtWaylandLinuxDmabuf *)object;
  ut_object_unref(self->formats);
  ut_object_unref(self->modifiers);
}

static UtWaylandObjectInterface wayland_object_interface = {
    .get_interface = ut_wayland_linux_dmabuf_get_interface,
    .get_id = ut_wayland_linux_dmabuf_get_id,
    .decode_event = ut_wayland_linux_dmabuf_decode_event};

static UtObjectInterface object_interface = {
    .type_name = "UtWaylandLinuxDmabuf",
    .init = ut_wayland_linux_dmabuf_init,
    .cleanup = ut_wayland_linux_dmabuf_cleanup,
    .interfaces = {{&ut_wayland_object_id, &wayland_object_interface},
                   {NULL, NULL}}};

UtObject *ut_wayland_linux_dmabuf_new(UtObject *client, uint32_t id) {
  UtObject *object =
      ut_object_new(sizeof(UtWaylandLinuxDmabuf), &object_interface);
  UtWaylandLinuxDmabuf *self = (UtWaylandLinuxDmabuf *)object;
  self->client = client;
  self->id = id;
  ut_wayland_client_register_object(client, object);
  return object;
}

UtObject *ut_wayland_linux_dmabuf_new_from_registry(UtObject *registry,
                                                    uint32_t name) {
  uint32_t id =
      ut_wayland_registry_bind(registry, name, "zwp_linux_dmabuf_v1", 4);
  return ut_wayland_linux_dmabuf_new(ut_wayland_registry_get_client(registry),
                                     id);
}

UtObject *ut_wayland_linux_dmabuf_get_formats(UtObject *object) {
  assert(ut_object_is_wayland_linux_dmabuf(object));
  UtWaylandLinuxDmabuf *self = (UtWaylandLinuxDmabuf *)object;
  return self->formats;
}

UtObject *ut_wayland_linux_dmabuf_get_modifiers(UtObject *object) {
  assert(ut_object_is_wayland_linux_dmabuf(object));
  UtWaylandLinuxDmabuf *self = (UtWaylandLinuxDmabuf *)object;
  return self->modifiers;
}

void ut_wayland_linux_dmabuf_destroy(UtObject *object) {
  assert(ut_object_is_wayland_linux_dmabuf(object));
  UtWaylandLinuxDmabuf *self = (UtWaylandLinuxDmabuf *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_client_send_request(self->client, self->id, 0,
                                 ut_wayland_encoder_get_data(payload));
}

UtObject *ut_wayland_linux_dmabuf_create_params(
    UtObject *object, UtObject *callback_object,
    const UtWaylandLinuxBufferParamsCallbacks *callbacks) {
  assert(ut_object_is_wayland_linux_dmabuf(object));
  UtWaylandLinuxDmabuf *self = (UtWaylandLinuxDmabuf *)object;

  uint32_t id = ut_wayland_client_allocate_id(self->client);
  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_uint(payload, id);
  ut_wayland_client_send_request(self->client, self->id, 1,
                                 ut_wayland_encoder_get_data(payload));
  UtObject *params = ut_wayland_linux_buffer_params_new(
      self->client, id, callback_object, callbacks);
  ut_wayland_client_register_object(self->client, params);
  return params;
}

bool ut_object_is_wayland_linux_dmabuf(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
