#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  uint16_t number;
  uint16_t scroll_type;
  uint32_t flags;
  double increment;
} UtX11ScrollClass;

static UtObjectInterface object_interface = {.type_name = "UtX11ScrollClass",
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_x11_scroll_class_new(uint16_t number, uint16_t scroll_type,
                                  uint32_t flags, double increment) {
  UtObject *object = ut_object_new(sizeof(UtX11ScrollClass), &object_interface);
  UtX11ScrollClass *self = (UtX11ScrollClass *)object;
  self->number = number;
  self->scroll_type = scroll_type;
  self->flags = flags;
  self->increment = increment;
  return object;
}

bool ut_object_is_x11_scroll_class(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
