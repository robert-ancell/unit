#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef void (*UtWaylandBufferReleaseCallback)(UtObject *object);

UtObject *
ut_wayland_buffer_new(UtObject *client, uint32_t id, UtObject *callback_object,
                      UtWaylandBufferReleaseCallback release_callback);

void ut_wayland_buffer_destroy(UtObject *object);

/// Returns [true] if [object] is a [UtWaylandBuffer].
bool ut_object_is_wayland_buffer(UtObject *object);
