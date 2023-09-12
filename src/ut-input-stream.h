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

void ut_input_stream_read(UtObject *object, UtObject *callback_object,
                          UtInputStreamCallback callback);

void ut_input_stream_read_all(UtObject *object, UtObject *callback_object,
                              UtInputStreamCallback callback);

UtObject *ut_input_stream_read_sync(UtObject *object);

void ut_input_stream_close(UtObject *object);

/// Returns [true] if [object] is a [UtInputStream].
bool ut_object_implements_input_stream(UtObject *object);
