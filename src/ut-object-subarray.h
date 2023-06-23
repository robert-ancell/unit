#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_object_subarray_new(UtObject *parent, size_t start, size_t length);

bool ut_object_is_object_subarray(UtObject *object);
