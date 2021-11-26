#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef void (*UtWaylandBufferReleaseCallback)(void *user_data);

UtObject *ut_wayland_buffer_new(UtObject *client, uint32_t id,
                                UtWaylandBufferReleaseCallback release_callback,
                                void *user_data);

void ut_wayland_buffer_destroy(UtObject *object);

bool ut_object_is_wayland_buffer(UtObject *object);
