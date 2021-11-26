#include <assert.h>

#include "ut-wayland-client-private.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *client;
  uint32_t id;
} UtWaylandRegion;

static const char *ut_wayland_region_get_interface(UtObject *object) {
  return "wl_region";
}

static uint32_t ut_wayland_region_get_id(UtObject *object) {
  UtWaylandRegion *self = (UtWaylandRegion *)object;
  return self->id;
}

static UtWaylandObjectInterface wayland_object_interface = {
    .get_interface = ut_wayland_region_get_interface,
    .get_id = ut_wayland_region_get_id};

static UtObjectInterface object_interface = {
    .type_name = "UtWaylandRegion",
    .interfaces = {{&ut_wayland_object_id, &wayland_object_interface},
                   {NULL, NULL}}};

UtObject *ut_wayland_region_new(UtObject *client, uint32_t id) {
  UtObject *object = ut_object_new(sizeof(UtWaylandRegion), &object_interface);
  UtWaylandRegion *self = (UtWaylandRegion *)object;
  self->client = client;
  self->id = id;
  ut_wayland_client_register_object(client, object);
  return object;
}

void ut_wayland_region_destroy(UtObject *object) {
  assert(ut_object_is_wayland_region(object));
  UtWaylandRegion *self = (UtWaylandRegion *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_client_send_request(self->client, self->id, 0,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_wayland_region_add(UtObject *object, int32_t x, int32_t y,
                           int32_t width, int32_t height) {
  assert(ut_object_is_wayland_region(object));
  UtWaylandRegion *self = (UtWaylandRegion *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_int(payload, x);
  ut_wayland_encoder_append_int(payload, y);
  ut_wayland_encoder_append_int(payload, width);
  ut_wayland_encoder_append_int(payload, height);
  ut_wayland_client_send_request(self->client, self->id, 1,
                                 ut_wayland_encoder_get_data(payload));
}

void ut_wayland_region_subtract(UtObject *object, int32_t x, int32_t y,
                                int32_t width, int32_t height) {
  assert(ut_object_is_wayland_region(object));
  UtWaylandRegion *self = (UtWaylandRegion *)object;

  UtObjectRef payload = ut_wayland_encoder_new();
  ut_wayland_encoder_append_int(payload, x);
  ut_wayland_encoder_append_int(payload, y);
  ut_wayland_encoder_append_int(payload, width);
  ut_wayland_encoder_append_int(payload, height);
  ut_wayland_client_send_request(self->client, self->id, 2,
                                 ut_wayland_encoder_get_data(payload));
}

bool ut_object_is_wayland_region(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
