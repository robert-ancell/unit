#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  int16_t x;
  int16_t y;
  uint16_t width;
  uint16_t height;
} UtX11Rectangle;

static UtObjectInterface object_interface = {.type_name = "UtX11Rectangle",
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_x11_rectangle_new(int16_t x, int16_t y, uint16_t width,
                               uint16_t height) {
  UtObject *object = ut_object_new(sizeof(UtX11Rectangle), &object_interface);
  UtX11Rectangle *self = (UtX11Rectangle *)object;
  self->x = x;
  self->y = y;
  self->width = width;
  self->height = height;
  return object;
}

void ut_x11_rectangle_get_dimensions(UtObject *object, int16_t *x, int16_t *y,
                                     uint16_t *width, uint16_t *height) {
  assert(ut_object_is_x11_rectangle(object));
  UtX11Rectangle *self = (UtX11Rectangle *)object;

  if (x != NULL) {
    *x = self->x;
  }
  if (y != NULL) {
    *y = self->y;
  }
  if (width != NULL) {
    *width = self->width;
  }
  if (height != NULL) {
    *height = self->height;
  }
}

bool ut_object_is_x11_rectangle(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
