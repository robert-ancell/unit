#include <stdbool.h>
#include <stddef.h>

#include "ut-object.h"

#pragma once

UtObject *ut_fd_input_stream_new(UtObject *fd);

bool ut_object_is_fd_input_stream(UtObject *object);
