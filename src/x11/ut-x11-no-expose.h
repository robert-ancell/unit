#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_x11_no_expose_new();

bool ut_object_is_x11_no_expose(UtObject *object);