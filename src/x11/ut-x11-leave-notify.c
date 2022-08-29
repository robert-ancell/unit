#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  uint32_t window;
  int16_t x;
  int16_t y;
} UtX11LeaveNotify;

static UtX11EventInterface x11_event_interface = {};

static UtObjectInterface object_interface = {
    .type_name = "UtX11LeaveNotify",
    .interfaces = {{&ut_x11_event_id, &x11_event_interface}}};

UtObject *ut_x11_leave_notify_new(uint32_t window, int16_t x, int16_t y) {
  UtObject *object = ut_object_new(sizeof(UtX11LeaveNotify), &object_interface);
  UtX11LeaveNotify *self = (UtX11LeaveNotify *)object;
  self->window = window;
  self->x = x;
  self->y = y;
  return object;
}

uint32_t ut_x11_leave_notify_get_window(UtObject *object) {
  assert(ut_object_is_x11_leave_notify(object));
  UtX11LeaveNotify *self = (UtX11LeaveNotify *)object;
  return self->window;
}

int16_t ut_x11_leave_notify_get_x(UtObject *object) {
  assert(ut_object_is_x11_leave_notify(object));
  UtX11LeaveNotify *self = (UtX11LeaveNotify *)object;
  return self->x;
}

int16_t ut_x11_leave_notify_get_y(UtObject *object) {
  assert(ut_object_is_x11_leave_notify(object));
  UtX11LeaveNotify *self = (UtX11LeaveNotify *)object;
  return self->y;
}

bool ut_object_is_x11_leave_notify(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
