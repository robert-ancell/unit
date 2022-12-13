#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_x11_button_class_new(UtObject *state, UtObject *labels);

bool ut_object_is_x11_button_class(UtObject *object);
