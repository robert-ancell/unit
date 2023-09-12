#include <stdbool.h>
#include <stddef.h>

#include "ut-object.h"

#pragma once

UtObject *ut_fd_output_stream_new(UtObject *fd);

/// Returns [true] if [object] is a [UtFdOutputStream].
bool ut_object_is_fd_output_stream(UtObject *object);
