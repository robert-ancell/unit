#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef void (*UtWaylandCallbackDoneCallback)(UtObject *object,
                                              uint32_t callback_data);

UtObject *ut_wayland_callback_new(UtObject *client, uint32_t id,
                                  UtObject *callback_object,
                                  UtWaylandCallbackDoneCallback done_callback);

bool ut_object_is_wayland_callback(UtObject *object);
