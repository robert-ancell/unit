#include <stdbool.h>
#include <stddef.h>

#include "ut-object.h"

#pragma once

typedef void (*UtWritableInputStreamReadingCallback)(UtObject *object,
                                                     UtObject *stream);

/// Creates a new input stream that can be written to.
///
/// !return-ref
/// !return-type UtWritableInputStream
UtObject *ut_writable_input_stream_new();

/// Set [reading_callback] to be called when this stream is read from.
void ut_writable_input_stream_set_reading_callback(
    UtObject *object, UtObject *callback_object,
    UtWritableInputStreamReadingCallback reading_callback);

/// Write [data] to the input stream.
/// If [complete] is set to true then no more data will be written after this
/// call. Returns the number of elements read from data, all other data should
/// be retained.
///
/// !arg-type data UtList
size_t ut_writable_input_stream_write(UtObject *object, UtObject *data,
                                      bool complete);

/// Returns [true] if [object] is a [UtWritableInputStream].
bool ut_object_is_writable_input_stream(UtObject *object);
