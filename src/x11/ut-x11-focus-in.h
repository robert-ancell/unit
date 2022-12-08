#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_x11_focus_in_new(uint32_t window);

uint32_t ut_x11_focus_in_get_window(UtObject *object);

bool ut_object_is_x11_focus_in(UtObject *object);
