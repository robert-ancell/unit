#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_deflate_error_new(const char *description);

bool ut_object_is_deflate_error(UtObject *object);
