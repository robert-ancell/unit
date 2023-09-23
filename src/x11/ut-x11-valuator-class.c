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

static UtObjectInterface object_interface = {.type_name = "UtX11ValuatorClass"};

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

uint16_t ut_x11_valuator_class_get_number(UtObject *object) {
  assert(ut_object_is_x11_valuator_class(object));
  UtX11ValuatorClass *self = (UtX11ValuatorClass *)object;
  return self->number;
}

uint32_t ut_x11_valuator_class_get_label(UtObject *object) {
  assert(ut_object_is_x11_valuator_class(object));
  UtX11ValuatorClass *self = (UtX11ValuatorClass *)object;
  return self->label;
}

double ut_x11_valuator_class_get_min(UtObject *object) {
  assert(ut_object_is_x11_valuator_class(object));
  UtX11ValuatorClass *self = (UtX11ValuatorClass *)object;
  return self->min;
}

double ut_x11_valuator_class_get_max(UtObject *object) {
  assert(ut_object_is_x11_valuator_class(object));
  UtX11ValuatorClass *self = (UtX11ValuatorClass *)object;
  return self->max;
}

double ut_x11_valuator_class_get_value(UtObject *object) {
  assert(ut_object_is_x11_valuator_class(object));
  UtX11ValuatorClass *self = (UtX11ValuatorClass *)object;
  return self->value;
}

uint32_t ut_x11_valuator_class_get_resolution(UtObject *object) {
  assert(ut_object_is_x11_valuator_class(object));
  UtX11ValuatorClass *self = (UtX11ValuatorClass *)object;
  return self->resolution;
}

uint8_t ut_x11_valuator_class_get_mode(UtObject *object) {
  assert(ut_object_is_x11_valuator_class(object));
  UtX11ValuatorClass *self = (UtX11ValuatorClass *)object;
  return self->mode;
}

bool ut_object_is_x11_valuator_class(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
