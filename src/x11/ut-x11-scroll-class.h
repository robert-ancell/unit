#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_x11_scroll_class_new(uint16_t number, uint16_t scroll_type,
                                  uint32_t flags, double increment);

/// Returns [true] if [object] is a [UtX11ScrollClass].
bool ut_object_is_x11_scroll_class(UtObject *object);
