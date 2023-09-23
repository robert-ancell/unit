#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new Wayland region object with [id] on [client].
///
/// !arg-type client UtWaylandClient
/// !return-ref
/// !return-type UtWaylandRegion
UtObject *ut_wayland_region_new(UtObject *client, uint32_t id);

/// Destroys this region.
void ut_wayland_region_destroy(UtObject *object);

/// Adds the rectangle of size [width]x[height] and position [x],[y] to this
/// region.
void ut_wayland_region_add(UtObject *object, int32_t x, int32_t y,
                           int32_t width, int32_t height);

/// Subtracts the rectangle of size [width]x[height] and position [x],[y] to
/// this region.
void ut_wayland_region_subtract(UtObject *object, int32_t x, int32_t y,
                                int32_t width, int32_t height);

/// Returns [true] if [object] is a [UtWaylandRegion].
bool ut_object_is_wayland_region(UtObject *object);
