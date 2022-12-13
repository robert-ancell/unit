#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  uint16_t device_id;
  uint64_t value;
} UtX11InputEventMask;

static UtObjectInterface object_interface = {.type_name = "UtX11InputEventMask",
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_x11_input_event_mask_new(uint16_t device_id, uint64_t value) {
  UtObject *object =
      ut_object_new(sizeof(UtX11InputEventMask), &object_interface);
  UtX11InputEventMask *self = (UtX11InputEventMask *)object;
  self->device_id = device_id;
  self->value = value;
  return object;
}

uint16_t ut_x11_input_event_mask_get_device_id(UtObject *object) {
  assert(ut_object_is_x11_input_event_mask(object));
  UtX11InputEventMask *self = (UtX11InputEventMask *)object;
  return self->device_id;
}

uint64_t ut_x11_input_event_mask_get_value(UtObject *object) {
  assert(ut_object_is_x11_input_event_mask(object));
  UtX11InputEventMask *self = (UtX11InputEventMask *)object;
  return self->value;
}

bool ut_object_is_x11_input_event_mask(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
