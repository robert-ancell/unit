#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  uint32_t window;
  uint8_t keycode;
  int16_t x;
  int16_t y;
} UtX11KeyPress;

static UtX11EventInterface x11_event_interface = {};

static UtObjectInterface object_interface = {
    .type_name = "UtX11KeyPress",
    .interfaces = {{&ut_x11_event_id, &x11_event_interface}, {NULL, NULL}}};

UtObject *ut_x11_key_press_new(uint32_t window, uint8_t keycode, int16_t x,
                               int16_t y) {
  UtObject *object = ut_object_new(sizeof(UtX11KeyPress), &object_interface);
  UtX11KeyPress *self = (UtX11KeyPress *)object;
  self->window = window;
  self->keycode = keycode;
  self->x = x;
  self->y = y;
  return object;
}

uint32_t ut_x11_key_press_get_window(UtObject *object) {
  assert(ut_object_is_x11_key_press(object));
  UtX11KeyPress *self = (UtX11KeyPress *)object;
  return self->window;
}

uint8_t ut_x11_key_press_get_keycode(UtObject *object) {
  assert(ut_object_is_x11_key_press(object));
  UtX11KeyPress *self = (UtX11KeyPress *)object;
  return self->keycode;
}

int16_t ut_x11_key_press_get_x(UtObject *object) {
  assert(ut_object_is_x11_key_press(object));
  UtX11KeyPress *self = (UtX11KeyPress *)object;
  return self->x;
}

int16_t ut_x11_key_press_get_y(UtObject *object) {
  assert(ut_object_is_x11_key_press(object));
  UtX11KeyPress *self = (UtX11KeyPress *)object;
  return self->y;
}

bool ut_object_is_x11_key_press(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
