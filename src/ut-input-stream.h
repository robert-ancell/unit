#include <stdbool.h>
#include <stddef.h>

#include "ut-object.h"

#pragma once

typedef size_t (*UtInputStreamDataCallback)(void *user_data, UtObject *data,
                                            bool complete);
typedef size_t (*UtInputStreamClosedCallback)(void *user_data, UtObject *data);

typedef struct {
  void (*read)(UtObject *object, UtInputStreamDataCallback callback,
               UtInputStreamClosedCallback closed_callback, void *user_data,
               UtObject *cancel);
  void (*set_active)(UtObject *object, bool active);
} UtInputStreamInterface;

extern int ut_input_stream_id;

void ut_input_stream_read(UtObject *object, UtInputStreamDataCallback callback,
                          UtInputStreamClosedCallback closed_callback,
                          void *user_data, UtObject *cancel);

void ut_input_stream_read_all(UtObject *object,
                              UtInputStreamDataCallback callback,
                              void *user_data, UtObject *cancel);

UtObject *ut_input_stream_read_sync(UtObject *object);

void ut_input_stream_set_active(UtObject *object, bool active);

bool ut_object_implements_input_stream(UtObject *object);
