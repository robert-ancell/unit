#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_list_input_stream_new(UtObject *list);

/// Returns [true] if [object] is a [UtListInputStream].
bool ut_object_is_list_input_stream(UtObject *object);
