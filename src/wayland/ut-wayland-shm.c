#include <assert.h>

#include "ut-wayland-client-private.h"
#include "ut-wayland-registry-private.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *client;
  uint32_t id;
  UtObject *formats;
} UtWaylandShm;

static void decode_format(UtWaylandShm *self, UtObject *data) {
  uint32_t format = ut_wayland_decoder_get_uint(data);
  ut_uint32_list_append(self->formats, format);
}

static const char *ut_wayland_shm_get_interface(UtObject *object) {
  return "wl_shm";
}

static uint32_t ut_wayland_shm_get_id(UtObject *object) {
  UtWaylandShm *self = (UtWaylandShm *)object;
  return self->id;
}

static bool ut_wayland_shm_decode_event(UtObject *object, uint16_t code,
                                        UtObject *data) {
  UtWaylandShm *self = (UtWaylandShm *)object;
  switch (code) {
  case 0:
    decode_format(self, data);
    return true;
  default:
    return false;
  }
}

static void ut_wayland_shm_init(UtObject *object) {
  UtWaylandShm *self = (UtWaylandShm *)object;
  self->formats = ut_uint32_list_new();
}

static void ut_wayland_shm_cleanup(UtObject *object) {
  UtWaylandShm *self = (UtWaylandShm *)object;
  ut_object_unref(self->formats);
}

static UtWaylandObjectInterface wayland_object_interface = {
    .get_interface = ut_wayland_shm_get_interface,
    .get_id = ut_wayland_shm_get_id,
    .decode_event = ut_wayland_shm_decode_event};

static UtObjectInterface object_interface = {
    .type_name = "UtWaylandShm",
    .init = ut_wayland_shm_init,
    .cleanup = ut_wayland_shm_cleanup,
    .interfaces = {{&ut_wayland_object_id, &wayland_object_interface},
                   {NULL, NULL}}};

UtObject *ut_wayland_shm_new(UtObject *client, uint32_t id) {
  UtObject *object = ut_object_new(sizeof(UtWaylandShm), &object_interface);
  UtWaylandShm *self = (UtWaylandShm *)object;
  self->client = client;
  self->id = id;
  ut_wayland_client_register_object(client, object);
  return object;
}

UtObject *ut_wayland_shm_new_from_registry(UtObject *registry, uint32_t name) {
  uint32_t id = ut_wayland_registry_bind(registry, name, "wl_shm", 1);
  return ut_wayland_shm_new(ut_wayland_registry_get_client(registry), id);
}

UtObject *ut_wayland_shm_get_formats(UtObject *object) {
  assert(ut_object_is_wayland_shm(object));
  UtWaylandShm *self = (UtWaylandShm *)object;
  return self->formats;
}

bool ut_wayland_shm_has_format(UtObject *object, uint32_t format) {
  assert(ut_object_is_wayland_shm(object));
  UtWaylandShm *self = (UtWaylandShm *)object;
  size_t formats_length = ut_list_get_length(self->formats);
  for (size_t i = 0; i < formats_length; i++) {
    if (ut_uint32_list_get_element(self->formats, i) == format) {
      return true;
    }
  }
  return false;
}

UtObject *ut_wayland_shm_create_pool(UtObject *object, UtObject *fd,
                                     int32_t size) {
  assert(ut_object_is_wayland_shm(object));
  UtWaylandShm *self = (UtWaylandShm *)object;

  uint32_t id = ut_wayland_client_allocate_id(self->client);
  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_uint(payload, id);
  ut_wayland_encoder_append_fd(payload, fd);
  ut_wayland_encoder_append_int(payload, size);
  ut_wayland_client_send_request(self->client, self->id, 0,
                                 ut_wayland_encoder_get_data(payload));
  UtObject *pool = ut_wayland_shm_pool_new(self->client, id);
  ut_wayland_client_register_object(self->client, pool);
  return pool;
}

bool ut_object_is_wayland_shm(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
