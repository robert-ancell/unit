#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_x11_unknown_input_class_new(uint16_t type);

bool ut_object_is_x11_unknown_input_class(UtObject *object);
