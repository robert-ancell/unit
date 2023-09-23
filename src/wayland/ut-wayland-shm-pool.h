#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new Wayland shared memory pool extension object with [id] on
/// [client].
///
/// !arg-type client UtWaylandClient
/// !return-ref
/// !return-type UtWaylandShmPool
UtObject *ut_wayland_shm_pool_new(UtObject *client, uint32_t id);

/// Creates a new buffer of size [width]x[height] in [format] from this pool.
/// The data for the buffer starts at [offset] bytes in the pool and each row is
/// length [stride] bytes. [release_callback] is called when this buffer is
/// released.
///
/// !return-ref
/// !return-type UtWaylandBuffer
UtObject *ut_wayland_shm_pool_create_buffer(
    UtObject *object, int32_t offset, int32_t width, int32_t height,
    int32_t stride, uint32_t format, UtObject *callback_object,
    UtWaylandBufferReleaseCallback release_callback);

/// Destroy this object.
void ut_wayland_shm_pool_destroy(UtObject *object);

/// Resize the shared memory pool to [size] bytes.
void ut_wayland_shm_pool_resize(UtObject *object, int32_t size);

/// Returns [true] if [object] is a [UtWaylandShmPool].
bool ut_object_is_wayland_shm_pool(UtObject *object);
