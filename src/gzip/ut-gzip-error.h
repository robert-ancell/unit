#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_gzip_error_new(const char *description);

bool ut_object_is_gzip_error(UtObject *object);
