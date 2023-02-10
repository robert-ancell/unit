#include <stdbool.h>
#include <stddef.h>

#include "ut-object.h"

#pragma once

typedef void (*UtBufferedInputStreamReadingCallback)(void *user_data,
                                                     UtObject *stream);

UtObject *ut_buffered_input_stream_new();

void ut_buffered_input_stream_write(UtObject *object, UtObject *data,
                                    bool complete);

bool ut_object_is_buffered_input_stream(UtObject *object);
