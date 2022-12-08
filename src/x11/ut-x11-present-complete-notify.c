#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  uint8_t kind;
  uint8_t mode;
  uint32_t event_id;
  uint32_t window;
  uint32_t serial;
  uint64_t ust;
  uint64_t msc;
} UtX11PresentCompleteNotify;

static UtX11EventInterface x11_event_interface = {};

static UtObjectInterface object_interface = {
    .type_name = "UtX11PresentCompleteNotify",
    .interfaces = {{&ut_x11_event_id, &x11_event_interface}}};

UtObject *ut_x11_present_complete_notify_new(uint8_t kind, uint8_t mode,
                                             uint32_t event_id, uint32_t window,
                                             uint32_t serial, uint64_t ust,
                                             uint64_t msc) {
  UtObject *object =
      ut_object_new(sizeof(UtX11PresentCompleteNotify), &object_interface);
  UtX11PresentCompleteNotify *self = (UtX11PresentCompleteNotify *)object;
  self->kind = kind;
  self->mode = mode;
  self->event_id = event_id;
  self->window = window;
  self->serial = serial;
  self->ust = ust;
  self->msc = msc;
  return object;
}

uint8_t ut_x11_present_complete_notify_get_kind(UtObject *object) {
  assert(ut_object_is_x11_present_complete_notify(object));
  UtX11PresentCompleteNotify *self = (UtX11PresentCompleteNotify *)object;
  return self->kind;
}

uint8_t ut_x11_present_complete_notify_get_mode(UtObject *object) {
  assert(ut_object_is_x11_present_complete_notify(object));
  UtX11PresentCompleteNotify *self = (UtX11PresentCompleteNotify *)object;
  return self->mode;
}

uint32_t ut_x11_present_complete_notify_get_event_id(UtObject *object) {
  assert(ut_object_is_x11_present_complete_notify(object));
  UtX11PresentCompleteNotify *self = (UtX11PresentCompleteNotify *)object;
  return self->event_id;
}

uint32_t ut_x11_present_complete_notify_get_window(UtObject *object) {
  assert(ut_object_is_x11_present_complete_notify(object));
  UtX11PresentCompleteNotify *self = (UtX11PresentCompleteNotify *)object;
  return self->window;
}

uint32_t ut_x11_present_complete_notify_get_serial(UtObject *object) {
  assert(ut_object_is_x11_present_complete_notify(object));
  UtX11PresentCompleteNotify *self = (UtX11PresentCompleteNotify *)object;
  return self->serial;
}

uint64_t ut_x11_present_complete_notify_get_ust(UtObject *object) {
  assert(ut_object_is_x11_present_complete_notify(object));
  UtX11PresentCompleteNotify *self = (UtX11PresentCompleteNotify *)object;
  return self->ust;
}

uint64_t ut_x11_present_complete_notify_get_msc(UtObject *object) {
  assert(ut_object_is_x11_present_complete_notify(object));
  UtX11PresentCompleteNotify *self = (UtX11PresentCompleteNotify *)object;
  return self->msc;
}

bool ut_object_is_x11_present_complete_notify(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
