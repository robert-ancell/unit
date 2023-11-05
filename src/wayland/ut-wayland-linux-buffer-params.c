#include <assert.h>

#include "ut-wayland-client-private.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *client;
  uint32_t id;
  UtObject *callback_object;
  const UtWaylandLinuxBufferParamsCallbacks *callbacks;
} UtWaylandLinuxBufferParams;

static void decode_created(UtWaylandLinuxBufferParams *self, UtObject *data) {
  uint32_t id = ut_wayland_decoder_get_uint(data);
  if (self->callback_object != NULL && self->callbacks != NULL &&
      self->callbacks->created != NULL) {
    self->callbacks->created(self->callback_object, id);
  }
}

static void decode_failed(UtWaylandLinuxBufferParams *self, UtObject *data) {
  if (self->callback_object != NULL && self->callbacks != NULL &&
      self->callbacks->failed != NULL) {
    self->callbacks->failed(self->callback_object);
  }
}

static const char *
ut_wayland_linux_buffer_params_get_interface(UtObject *object) {
  return "zwp_linux_buffer_params_v1";
}

static uint32_t ut_wayland_linux_buffer_params_get_id(UtObject *object) {
  UtWaylandLinuxBufferParams *self = (UtWaylandLinuxBufferParams *)object;
  return self->id;
}

static bool ut_wayland_linux_buffer_params_decode_event(UtObject *object,
                                                        uint16_t code,
                                                        UtObject *data) {
  UtWaylandLinuxBufferParams *self = (UtWaylandLinuxBufferParams *)object;
  switch (code) {
  case 0:
    decode_created(self, data);
    return true;
  case 1:
    decode_failed(self, data);
    return true;
  default:
    return false;
  }
}

static void ut_wayland_linux_buffer_params_cleanup(UtObject *object) {
  UtWaylandLinuxBufferParams *self = (UtWaylandLinuxBufferParams *)object;
  ut_object_weak_unref(&self->callback_object);
}

static UtWaylandObjectInterface wayland_object_interface = {
    .get_interface = ut_wayland_linux_buffer_params_get_interface,
    .get_id = ut_wayland_linux_buffer_params_get_id,
    .decode_event = ut_wayland_linux_buffer_params_decode_event};

static UtObjectInterface object_interface = {
    .type_name = "UtWaylandLinuxBufferParams",
    .cleanup = ut_wayland_linux_buffer_params_cleanup,
    .interfaces = {{&ut_wayland_object_id, &wayland_object_interface},
                   {NULL, NULL}}};

UtObject *ut_wayland_linux_buffer_params_new(
    UtObject *client, uint32_t id, UtObject *callback_object,
    const UtWaylandLinuxBufferParamsCallbacks *callbacks) {
  UtObject *object =
      ut_object_new(sizeof(UtWaylandLinuxBufferParams), &object_interface);
  UtWaylandLinuxBufferParams *self = (UtWaylandLinuxBufferParams *)object;
  self->client = client;
  self->id = id;
  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callbacks = callbacks;
  ut_wayland_client_register_object(client, object);
  return object;
}

void ut_wayland_linux_buffer_params_destroy(UtObject *object) {
  assert(ut_object_is_wayland_linux_buffer_params(object));
  UtWaylandLinuxBufferParams *self = (UtWaylandLinuxBufferParams *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_client_send_request(self->client, self->id, 0,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_wayland_linux_buffer_params_add(UtObject *object, UtObject *fd,
                                        uint32_t plane_index, uint32_t offset,
                                        uint32_t stride, uint64_t modifier) {
  assert(ut_object_is_wayland_linux_buffer_params(object));
  UtWaylandLinuxBufferParams *self = (UtWaylandLinuxBufferParams *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_fd(payload, fd);
  ut_wayland_encoder_append_uint(payload, plane_index);
  ut_wayland_encoder_append_uint(payload, offset);
  ut_wayland_encoder_append_uint(payload, stride);
  ut_wayland_encoder_append_uint(payload, modifier >> 32);
  ut_wayland_encoder_append_uint(payload, modifier & 0xffffffff);
  ut_wayland_client_send_request(self->client, self->id, 1,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_wayland_linux_buffer_params_create(UtObject *object, int32_t width,
                                           int32_t height, uint32_t format,
                                           uint32_t flags) {
  assert(ut_object_is_wayland_linux_buffer_params(object));
  UtWaylandLinuxBufferParams *self = (UtWaylandLinuxBufferParams *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_int(payload, width);
  ut_wayland_encoder_append_int(payload, height);
  ut_wayland_encoder_append_uint(payload, format);
  ut_wayland_encoder_append_uint(payload, flags);
  ut_wayland_client_send_request(self->client, self->id, 2,
                                 ut_wayland_encoder_get_data(payload));
}

bool ut_object_is_wayland_linux_buffer_params(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
