#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_gif_error_new(const char *description);

bool ut_object_is_gif_error(UtObject *object);
