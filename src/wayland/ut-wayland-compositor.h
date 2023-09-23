#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"
#include "ut-wayland-surface.h"

#pragma once

/// Creates a new Wayland compositor object with [id] on [client].
///
/// !arg-type client UtWaylandClient
/// !return-ref
/// !return-type UtWaylandCompositor
UtObject *ut_wayland_compositor_new(UtObject *client, uint32_t id);

/// Creates a new Wayland compositor object with well known [name] from
/// [registry].
///
/// !arg-type registry UtWaylandRegistry
/// !return-ref
/// !return-type UtWaylandCompositor
UtObject *ut_wayland_compositor_new_from_registry(UtObject *registry,
                                                  uint32_t name);

/// Returns a new surface.
/// The functions in [callbacks] will be called for any events for this surface.
///
/// !return-ref
/// !return-type UtWaylandSurface
UtObject *ut_wayland_compositor_create_surface(
    UtObject *object, UtObject *callback_object,
    const UtWaylandSurfaceCallbacks *callbacks);

/// Creates a region.
///
/// !return-ref
/// !return-type UtWaylandRegion
UtObject *ut_wayland_compositor_create_region(UtObject *object);

/// Returns [true] if [object] is a [UtWaylandCompositor].
bool ut_object_is_wayland_compositor(UtObject *object);
