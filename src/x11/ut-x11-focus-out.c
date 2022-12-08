#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  uint32_t window;
} UtX11FocusOut;

static UtX11EventInterface x11_event_interface = {};

static UtObjectInterface object_interface = {
    .type_name = "UtX11FocusOut",
    .interfaces = {{&ut_x11_event_id, &x11_event_interface}}};

UtObject *ut_x11_focus_out_new(uint32_t window) {
  UtObject *object = ut_object_new(sizeof(UtX11FocusOut), &object_interface);
  UtX11FocusOut *self = (UtX11FocusOut *)object;
  self->window = window;
  return object;
}

uint32_t ut_x11_focus_out_get_window(UtObject *object) {
  assert(ut_object_is_x11_focus_out(object));
  UtX11FocusOut *self = (UtX11FocusOut *)object;
  return self->window;
}

bool ut_object_is_x11_focus_out(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
