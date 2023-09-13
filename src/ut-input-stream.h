#include <stdbool.h>
#include <stddef.h>

#include "ut-object.h"

#pragma once

typedef size_t (*UtInputStreamCallback)(UtObject *object, UtObject *data,
                                        bool complete);

typedef struct {
  void (*read)(UtObject *object, UtObject *callback_object,
               UtInputStreamCallback callback);
  void (*close)(UtObject *object);
} UtInputStreamInterface;

extern int ut_input_stream_id;

/// Start reading from this stream.
/// [callback] is called when data is available.
void ut_input_stream_read(UtObject *object, UtObject *callback_object,
                          UtInputStreamCallback callback);

/// Read all the data from this stream and call [callback] when complete.
void ut_input_stream_read_all(UtObject *object, UtObject *callback_object,
                              UtInputStreamCallback callback);

/// Returns all data from this stream.
/// Returns an error if all the stream data is not available.
///
/// !return-ref
/// !return-type UtList UtError
UtObject *ut_input_stream_read_sync(UtObject *object);

/// Close this stream.
/// The callback from [ut_input_stream_read] will no longer be called.
void ut_input_stream_close(UtObject *object);

/// Returns [true] if [object] is a [UtInputStream].
bool ut_object_implements_input_stream(UtObject *object);
