#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  uint8_t mode;
  uint8_t num_touches;
} UtX11TouchClass;

static UtObjectInterface object_interface = {.type_name = "UtX11TouchClass",
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_x11_touch_class_new(uint8_t mode, uint8_t num_touches) {
  UtObject *object = ut_object_new(sizeof(UtX11TouchClass), &object_interface);
  UtX11TouchClass *self = (UtX11TouchClass *)object;
  self->mode = mode;
  self->num_touches = num_touches;
  return object;
}

bool ut_object_is_x11_touch_class(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
