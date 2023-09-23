#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef void (*UtWaylandBufferReleaseCallback)(UtObject *object);

/// Creates a new Wayland buffer object with [id] on [client].
/// [release_callback] will be called when this buffer is released.
///
/// !arg-type client UtWaylandClient
/// !return-ref
/// !return-type UtWaylandBuffer
UtObject *
ut_wayland_buffer_new(UtObject *client, uint32_t id, UtObject *callback_object,
                      UtWaylandBufferReleaseCallback release_callback);

/// Destroys this buffer.
void ut_wayland_buffer_destroy(UtObject *object);

/// Returns [true] if [object] is a [UtWaylandBuffer].
bool ut_object_is_wayland_buffer(UtObject *object);
