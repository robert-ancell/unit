#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_float32_new(float value);

float ut_float32_get_value(UtObject *object);

bool ut_object_is_float32(UtObject *object);
