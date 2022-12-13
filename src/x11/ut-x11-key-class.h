#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_x11_key_class_new(UtObject *keys);

bool ut_object_is_x11_key_class(UtObject *object);
