#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  uint32_t window;
} UtX11FocusIn;

static UtX11EventInterface x11_event_interface = {};

static UtObjectInterface object_interface = {
    .type_name = "UtX11FocusIn",
    .interfaces = {{&ut_x11_event_id, &x11_event_interface}}};

UtObject *ut_x11_focus_in_new(uint32_t window) {
  UtObject *object = ut_object_new(sizeof(UtX11FocusIn), &object_interface);
  UtX11FocusIn *self = (UtX11FocusIn *)object;
  self->window = window;
  return object;
}

uint32_t ut_x11_focus_in_get_window(UtObject *object) {
  assert(ut_object_is_x11_focus_in(object));
  UtX11FocusIn *self = (UtX11FocusIn *)object;
  return self->window;
}

bool ut_object_is_x11_focus_in(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
