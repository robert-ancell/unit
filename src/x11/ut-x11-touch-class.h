#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_x11_touch_class_new(uint8_t mode, uint8_t num_touches);

bool ut_object_is_x11_touch_class(UtObject *object);
