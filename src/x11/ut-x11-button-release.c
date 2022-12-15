#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  uint32_t window;
  uint8_t button;
  int16_t x;
  int16_t y;
} UtX11ButtonRelease;

static UtX11EventInterface x11_event_interface = {};

static UtObjectInterface object_interface = {
    .type_name = "UtX11ButtonRelease",
    .interfaces = {{&ut_x11_event_id, &x11_event_interface}, {NULL, NULL}}};

UtObject *ut_x11_button_release_new(uint32_t window, uint8_t button, int16_t x,
                                    int16_t y) {
  UtObject *object =
      ut_object_new(sizeof(UtX11ButtonRelease), &object_interface);
  UtX11ButtonRelease *self = (UtX11ButtonRelease *)object;
  self->window = window;
  self->button = button;
  self->x = x;
  self->y = y;
  return object;
}

uint32_t ut_x11_button_release_get_window(UtObject *object) {
  assert(ut_object_is_x11_button_release(object));
  UtX11ButtonRelease *self = (UtX11ButtonRelease *)object;
  return self->window;
}

uint8_t ut_x11_button_release_get_button(UtObject *object) {
  assert(ut_object_is_x11_button_release(object));
  UtX11ButtonRelease *self = (UtX11ButtonRelease *)object;
  return self->button;
}

int16_t ut_x11_button_release_get_x(UtObject *object) {
  assert(ut_object_is_x11_button_release(object));
  UtX11ButtonRelease *self = (UtX11ButtonRelease *)object;
  return self->x;
}

int16_t ut_x11_button_release_get_y(UtObject *object) {
  assert(ut_object_is_x11_button_release(object));
  UtX11ButtonRelease *self = (UtX11ButtonRelease *)object;
  return self->y;
}

bool ut_object_is_x11_button_release(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
