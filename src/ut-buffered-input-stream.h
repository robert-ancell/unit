#include <stdbool.h>
#include <stddef.h>

#include "ut-object.h"

#pragma once

/// Create a new input stream where unread data is buffered.
///
/// !return-ref
/// !return-type UtBufferedInputStream
UtObject *ut_buffered_input_stream_new();

/// Write [data] to this stream.
/// If [complete] is set to true then no more data will be written after this
/// call.
///
/// !arg-type data UtList
void ut_buffered_input_stream_write(UtObject *object, UtObject *data,
                                    bool complete);

/// Returns [true] if [object] is a [UtBufferedInputStream].
bool ut_object_is_buffered_input_stream(UtObject *object);
