#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new XDG positioner object.
///
/// !arg-type client UtWaylandClient
/// !return-ref
/// !return-type UtXdgPositioner
UtObject *ut_xdg_positioner_new(UtObject *client, uint32_t id);

/// Destroy this positioner.
void ut_xdg_positioner_destroy(UtObject *object);

/// Sets the size of this surface that is to be positioned to to
/// [width]x[height]
void ut_xdg_positioner_set_size(UtObject *object, int32_t width,
                                int32_t height);

/// Sets the anchor rectangle within the parent surface to [width]x[height] at
/// [x],[y].
void ut_xdg_positioner_set_anchor_rect(UtObject *object, int32_t x, int32_t y,
                                       int32_t width, int32_t height);

/// Sets the [anchor] for this positioner.
void ut_xdg_positioner_set_anchor(UtObject *object, uint32_t anchor);

/// Sets the [gravity] for this positioner.
void ut_xdg_positioner_set_gravity(UtObject *object, uint32_t gravity);

/// Sets the adjustment to be done when constrained.
void ut_xdg_positioner_set_constraint_adjustment(
    UtObject *object, uint32_t constraint_adjustment);

/// Sets the [x],[y] offset for this positioner.
void ut_xdg_positioner_set_offset(UtObject *object, int32_t x, int32_t y);

/// Marks this positioner as reactive.
void ut_xdg_positioner_set_reactive(UtObject *object);

/// Sets the parent window geometry to [width]x[height].
void ut_xdg_positioner_set_parent_size(UtObject *object, int32_t width,
                                       int32_t height);

/// Sets the [serial] number of the configure event this positioner is in
/// response to.
void ut_xdg_positioner_set_parent_configure(UtObject *object, uint32_t serial);

/// Returns [true] if [object] is a [UtXdgPositioner].
bool ut_object_is_xdg_positioner(UtObject *object);
