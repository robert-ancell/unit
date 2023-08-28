#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_protobuf_error_new(const char *description);

bool ut_object_is_protobuf_error(UtObject *object);
