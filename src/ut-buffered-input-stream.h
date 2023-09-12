#include <stdbool.h>
#include <stddef.h>

#include "ut-object.h"

#pragma once

UtObject *ut_buffered_input_stream_new();

void ut_buffered_input_stream_write(UtObject *object, UtObject *data,
                                    bool complete);

/// Returns [true] if [object] is a [UtBufferedInputStream].
bool ut_object_is_buffered_input_stream(UtObject *object);
