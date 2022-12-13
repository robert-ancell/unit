#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  uint16_t number;
  uint32_t label;
  double min;
  double max;
  double value;
  uint32_t resolution;
  uint8_t mode;
} UtX11ValuatorClass;

static UtObjectInterface object_interface = {.type_name = "UtX11ValuatorClass",
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_x11_valuator_class_new(uint16_t number, uint32_t label, double min,
                                    double max, double value,
                                    uint32_t resolution, uint8_t mode) {
  UtObject *object =
      ut_object_new(sizeof(UtX11ValuatorClass), &object_interface);
  UtX11ValuatorClass *self = (UtX11ValuatorClass *)object;
  self->number = number;
  self->label = label;
  self->min = min;
  self->max = max;
  self->value = value;
  self->resolution = resolution;
  self->mode = mode;
  return object;
}

bool ut_object_is_x11_valuator_class(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
