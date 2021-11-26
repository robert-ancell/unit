#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef void (*UtWaylandCallbackDoneCallback)(void *user_data,
                                              uint32_t callback_data);

UtObject *ut_wayland_callback_new(UtObject *client, uint32_t id,
                                  UtWaylandCallbackDoneCallback done_callback,
                                  void *user_data);

bool ut_object_is_wayland_callback(UtObject *object);
