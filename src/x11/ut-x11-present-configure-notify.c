#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  uint32_t event_id;
  uint32_t window;
  int16_t x;
  int16_t y;
  uint16_t width;
  uint16_t height;
  int16_t off_x;
  int16_t off_y;
  uint16_t pixmap_width;
  uint16_t pixmap_height;
  uint32_t pixmap_flags;
} UtX11PresentConfigureNotify;

static UtX11EventInterface x11_event_interface = {};

static UtObjectInterface object_interface = {
    .type_name = "UtX11PresentConfigureNotify",
    .interfaces = {{&ut_x11_event_id, &x11_event_interface}}};

UtObject *ut_x11_present_configure_notify_new(
    uint32_t event_id, uint32_t window, int16_t x, int16_t y, uint16_t width,
    uint16_t height, int16_t off_x, int16_t off_y, uint16_t pixmap_width,
    uint16_t pixmap_height, uint32_t pixmap_flags) {
  UtObject *object =
      ut_object_new(sizeof(UtX11PresentConfigureNotify), &object_interface);
  UtX11PresentConfigureNotify *self = (UtX11PresentConfigureNotify *)object;
  self->event_id = event_id;
  self->window = window;
  self->x = x;
  self->y = y;
  self->width = width;
  self->height = height;
  self->off_x = off_x;
  self->off_y = off_y;
  self->pixmap_width = pixmap_width;
  self->pixmap_height = pixmap_height;
  self->pixmap_flags = pixmap_flags;
  return object;
}

uint32_t ut_x11_present_configure_notify_get_event_id(UtObject *object) {
  assert(ut_object_is_x11_present_configure_notify(object));
  UtX11PresentConfigureNotify *self = (UtX11PresentConfigureNotify *)object;
  return self->event_id;
}

uint32_t ut_x11_present_configure_notify_get_window(UtObject *object) {
  assert(ut_object_is_x11_present_configure_notify(object));
  UtX11PresentConfigureNotify *self = (UtX11PresentConfigureNotify *)object;
  return self->window;
}

int16_t ut_x11_present_configure_notify_get_x(UtObject *object) {
  assert(ut_object_is_x11_present_configure_notify(object));
  UtX11PresentConfigureNotify *self = (UtX11PresentConfigureNotify *)object;
  return self->x;
}

int16_t ut_x11_present_configure_notify_get_y(UtObject *object) {
  assert(ut_object_is_x11_present_configure_notify(object));
  UtX11PresentConfigureNotify *self = (UtX11PresentConfigureNotify *)object;
  return self->y;
}

uint16_t ut_x11_present_configure_notify_get_width(UtObject *object) {
  assert(ut_object_is_x11_present_configure_notify(object));
  UtX11PresentConfigureNotify *self = (UtX11PresentConfigureNotify *)object;
  return self->width;
}

uint16_t ut_x11_present_configure_notify_get_height(UtObject *object) {
  assert(ut_object_is_x11_present_configure_notify(object));
  UtX11PresentConfigureNotify *self = (UtX11PresentConfigureNotify *)object;
  return self->height;
}

int16_t ut_x11_present_configure_notify_get_off_x(UtObject *object) {
  assert(ut_object_is_x11_present_configure_notify(object));
  UtX11PresentConfigureNotify *self = (UtX11PresentConfigureNotify *)object;
  return self->off_x;
}

int16_t ut_x11_present_configure_notify_get_off_y(UtObject *object) {
  assert(ut_object_is_x11_present_configure_notify(object));
  UtX11PresentConfigureNotify *self = (UtX11PresentConfigureNotify *)object;
  return self->off_y;
}

uint16_t ut_x11_present_configure_notify_get_pixmap_width(UtObject *object) {
  assert(ut_object_is_x11_present_configure_notify(object));
  UtX11PresentConfigureNotify *self = (UtX11PresentConfigureNotify *)object;
  return self->pixmap_width;
}

uint16_t ut_x11_present_configure_notify_get_pixmap_height(UtObject *object) {
  assert(ut_object_is_x11_present_configure_notify(object));
  UtX11PresentConfigureNotify *self = (UtX11PresentConfigureNotify *)object;
  return self->pixmap_height;
}

uint32_t ut_x11_present_configure_notify_get_pixmap_flags(UtObject *object) {
  assert(ut_object_is_x11_present_configure_notify(object));
  UtX11PresentConfigureNotify *self = (UtX11PresentConfigureNotify *)object;
  return self->pixmap_flags;
}

bool ut_object_is_x11_present_configure_notify(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
