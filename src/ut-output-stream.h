#include <stddef.h>

#include "ut-object.h"

#pragma once

typedef void (*UtOutputStreamCallback)(UtObject *object, UtObject *error);

typedef struct {
  void (*write)(UtObject *object, UtObject *data, UtObject *callback_object,
                UtOutputStreamCallback callback);
} UtOutputStreamInterface;

extern int ut_output_stream_id;

/// Write [data] to this stream.
///
/// !arg-type data UtList
void ut_output_stream_write(UtObject *object, UtObject *data);

/// Write [data] to this stream.
/// [callback] is called when this data has been processed.
///
/// !arg-type data UtList
void ut_output_stream_write_full(UtObject *object, UtObject *data,
                                 UtObject *callback_object,
                                 UtOutputStreamCallback callback);

/// Returns [true] if [object] is a [UtOutputStream].
bool ut_object_implements_output_stream(UtObject *object);
