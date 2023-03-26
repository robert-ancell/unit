#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_exif_error_new(const char *description);

bool ut_object_is_exif_error(UtObject *object);
