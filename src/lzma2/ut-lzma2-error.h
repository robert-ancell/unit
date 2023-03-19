#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_lzma2_error_new(const char *description);

bool ut_object_is_lzma2_error(UtObject *object);
