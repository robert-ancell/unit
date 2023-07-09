#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_system_error_new(int code);

int ut_system_error_getcode(UtObject *object);

bool ut_object_is_system_error(UtObject *object);
