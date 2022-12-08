#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  uint32_t event_id;
  uint32_t window;
  uint32_t serial;
  uint32_t pixmap;
  uint32_t idle_fence;
} UtX11PresentIdleNotify;

static UtX11EventInterface x11_event_interface = {};

static UtObjectInterface object_interface = {
    .type_name = "UtX11PresentIdleNotify",
    .interfaces = {{&ut_x11_event_id, &x11_event_interface}}};

UtObject *ut_x11_present_idle_notify_new(uint32_t event_id, uint32_t window,
                                         uint32_t serial, uint32_t pixmap,
                                         uint32_t idle_fence) {
  UtObject *object =
      ut_object_new(sizeof(UtX11PresentIdleNotify), &object_interface);
  UtX11PresentIdleNotify *self = (UtX11PresentIdleNotify *)object;
  self->event_id = event_id;
  self->window = window;
  self->serial = serial;
  self->pixmap = pixmap;
  self->idle_fence = idle_fence;
  return object;
}

uint32_t ut_x11_present_idle_notify_get_event_id(UtObject *object) {
  assert(ut_object_is_x11_present_idle_notify(object));
  UtX11PresentIdleNotify *self = (UtX11PresentIdleNotify *)object;
  return self->event_id;
}

uint32_t ut_x11_present_idle_notify_get_window(UtObject *object) {
  assert(ut_object_is_x11_present_idle_notify(object));
  UtX11PresentIdleNotify *self = (UtX11PresentIdleNotify *)object;
  return self->window;
}

uint32_t ut_x11_present_idle_notify_get_serial(UtObject *object) {
  assert(ut_object_is_x11_present_idle_notify(object));
  UtX11PresentIdleNotify *self = (UtX11PresentIdleNotify *)object;
  return self->serial;
}

uint32_t ut_x11_present_idle_notify_get_pixmap(UtObject *object) {
  assert(ut_object_is_x11_present_idle_notify(object));
  UtX11PresentIdleNotify *self = (UtX11PresentIdleNotify *)object;
  return self->pixmap;
}

uint32_t ut_x11_present_idle_notify_get_idle_fence(UtObject *object) {
  assert(ut_object_is_x11_present_idle_notify(object));
  UtX11PresentIdleNotify *self = (UtX11PresentIdleNotify *)object;
  return self->idle_fence;
}

bool ut_object_is_x11_present_idle_notify(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
