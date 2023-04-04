#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_http_error_new(const char *description);

bool ut_object_is_http_error(UtObject *object);
