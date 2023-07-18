#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_wayland_shm_pool_new(UtObject *client, uint32_t id);

UtObject *ut_wayland_shm_pool_create_buffer(
    UtObject *object, int32_t offset, int32_t width, int32_t height,
    int32_t stride, uint32_t format, UtObject *callback_object,
    UtWaylandBufferReleaseCallback release_callback);

void ut_wayland_shm_pool_destroy(UtObject *object);

void ut_wayland_shm_pool_resize(UtObject *object, int32_t size);

bool ut_object_is_wayland_shm_pool(UtObject *object);
