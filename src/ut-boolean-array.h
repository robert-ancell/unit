#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_boolean_array_new();

UtObject *ut_boolean_array_new_sized(size_t length);

bool *ut_boolean_array_get_data(UtObject *object);

bool ut_object_is_boolean_array(UtObject *object);
