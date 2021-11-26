#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"
#include "ut-wayland-surface.h"

#pragma once

UtObject *ut_wayland_compositor_new(UtObject *client, uint32_t id);

UtObject *ut_wayland_compositor_new_from_registry(UtObject *registry,
                                                  uint32_t name);

UtObject *
ut_wayland_compositor_create_surface(UtObject *object,
                                     const UtWaylandSurfaceCallbacks *callbacks,
                                     void *user_data);

UtObject *ut_wayland_compositor_create_region(UtObject *object);

bool ut_object_is_wayland_compositor(UtObject *object);
