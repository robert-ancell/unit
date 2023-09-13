#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_x11_valuator_class_new(uint16_t number, uint32_t label, double min,
                                    double max, double value,
                                    uint32_t resolution, uint8_t mode);

/// Returns [true] if [object] is a [UtX11ValuatorClass].
bool ut_object_is_x11_valuator_class(UtObject *object);
