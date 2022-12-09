#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  uint32_t segment;
} UtX11BadRegionError;

static UtX11ErrorInterface x11_error_interface = {};

static UtObjectInterface object_interface = {
    .type_name = "UtX11BadRegionError",
    .interfaces = {{&ut_x11_error_id, &x11_error_interface}}};

UtObject *ut_x11_bad_region_error_new(uint32_t segment) {
  UtObject *object =
      ut_object_new(sizeof(UtX11BadRegionError), &object_interface);
  UtX11BadRegionError *self = (UtX11BadRegionError *)object;
  self->segment = segment;
  return object;
}

uint32_t ut_x11_bad_region_error_get_segment(UtObject *object) {
  assert(ut_object_is_x11_bad_region_error(object));
  UtX11BadRegionError *self = (UtX11BadRegionError *)object;
  return self->segment;
}

bool ut_object_is_x11_bad_region_error(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
