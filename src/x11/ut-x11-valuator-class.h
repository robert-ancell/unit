#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new X11 valuator with [number], [label], [min], [max], [value],
/// [resolution] and [mode].
///
/// !return-ref
/// !return-type UtX11Valuator
UtObject *ut_x11_valuator_class_new(uint16_t number, uint32_t label, double min,
                                    double max, double value,
                                    uint32_t resolution, uint8_t mode);

/// Returns the number for this class.
uint16_t ut_x11_valuator_class_get_number(UtObject *object);

/// Returns the label atom for this class.
uint32_t ut_x11_valuator_class_get_label(UtObject *object);

/// Returns the minimum value this class can use.
double ut_x11_valuator_class_get_min(UtObject *object);

/// Returns the maximum value this class can use.
double ut_x11_valuator_class_get_max(UtObject *object);

/// Returns the current value for this class.
double ut_x11_valuator_class_get_value(UtObject *object);

/// Returns the resolution of the value in this class.
uint32_t ut_x11_valuator_class_get_resolution(UtObject *object);

/// Returns the mode for this class.
uint8_t ut_x11_valuator_class_get_mode(UtObject *object);

/// Returns [true] if [object] is a [UtX11ValuatorClass].
bool ut_object_is_x11_valuator_class(UtObject *object);
