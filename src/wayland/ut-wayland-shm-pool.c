#include <assert.h>

#include "ut-wayland-client-private.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *client;
  uint32_t id;
} UtWaylandShmPool;

static const char *ut_wayland_shm_pool_get_interface(UtObject *object) {
  return "wl_shm_pool";
}

static uint32_t ut_wayland_shm_pool_get_id(UtObject *object) {
  UtWaylandShmPool *self = (UtWaylandShmPool *)object;
  return self->id;
}

static UtWaylandObjectInterface wayland_object_interface = {
    .get_interface = ut_wayland_shm_pool_get_interface,
    .get_id = ut_wayland_shm_pool_get_id};

static UtObjectInterface object_interface = {
    .type_name = "UtWaylandShmPool",
    .interfaces = {{&ut_wayland_object_id, &wayland_object_interface},
                   {NULL, NULL}}};

UtObject *ut_wayland_shm_pool_new(UtObject *client, uint32_t id) {
  UtObject *object = ut_object_new(sizeof(UtWaylandShmPool), &object_interface);
  UtWaylandShmPool *self = (UtWaylandShmPool *)object;
  self->client = client;
  self->id = id;
  ut_wayland_client_register_object(client, object);
  return object;
}

UtObject *ut_wayland_shm_pool_create_buffer(
    UtObject *object, int32_t offset, int32_t width, int32_t height,
    int32_t stride, uint32_t format,
    UtWaylandBufferReleaseCallback release_callback, void *user_data) {
  assert(ut_object_is_wayland_shm_pool(object));
  UtWaylandShmPool *self = (UtWaylandShmPool *)object;

  uint32_t id = ut_wayland_client_allocate_id(self->client);
  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_uint(payload, id);
  ut_wayland_encoder_append_int(payload, offset);
  ut_wayland_encoder_append_int(payload, width);
  ut_wayland_encoder_append_int(payload, height);
  ut_wayland_encoder_append_int(payload, stride);
  ut_wayland_encoder_append_uint(payload, format);
  ut_wayland_client_send_request(self->client, self->id, 0,
                                 ut_wayland_encoder_get_data(payload));
  UtObject *buffer =
      ut_wayland_buffer_new(self->client, id, release_callback, user_data);
  ut_wayland_client_register_object(self->client, buffer);
  return buffer;
}

void ut_wayland_shm_pool_destroy(UtObject *object) {
  assert(ut_object_is_wayland_shm_pool(object));
  UtWaylandShmPool *self = (UtWaylandShmPool *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_client_send_request(self->client, self->id, 1,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_wayland_shm_pool_resize(UtObject *object, int32_t size) {
  assert(ut_object_is_wayland_shm_pool(object));
  UtWaylandShmPool *self = (UtWaylandShmPool *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_int(payload, size);
  ut_wayland_client_send_request(self->client, self->id, 2,
                                 ut_wayland_encoder_get_data(payload));
}

bool ut_object_is_wayland_shm_pool(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
