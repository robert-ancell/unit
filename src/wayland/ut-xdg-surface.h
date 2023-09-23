#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"
#include "ut-xdg-popup.h"
#include "ut-xdg-toplevel.h"

#pragma once

typedef void (*UtXdgSurfaceConfigureCallback)(UtObject *object,
                                              uint32_t serial);

typedef struct {
  UtXdgSurfaceConfigureCallback configure;
} UtXdgSurfaceCallbacks;

/// Creates a new XDG surface object with [id] on [client].
/// The functions in [callbacks] will be called for any events for this object.
///
/// !arg-type client UtWaylandClient
/// !return-ref
/// !return-type UtXdgSurface
UtObject *ut_xdg_surface_new(UtObject *client, uint32_t id,
                             UtObject *callback_object,
                             const UtXdgSurfaceCallbacks *callbacks);

/// Destroy this surface.
void ut_xdg_surface_destroy(UtObject *object);

/// Creates a toplevel surface object for this surface.
/// The functions in [callbacks] will be called for any events for this object.
///
/// !return-ref
/// !return-type UtXdgToplevel
UtObject *ut_xdg_surface_get_toplevel(UtObject *object,
                                      UtObject *callback_object,
                                      const UtXdgToplevelCallbacks *callbacks);

/// Creates a popup surface object for this surface.
/// The popup comes from [parent] and is placed according to [positioner].
/// The functions in [callbacks] will be called for any events for this object.
///
/// !arg-type parent UtXdgSurface
/// !arg-type positioner UtXdgPositioner
/// !return-ref
/// !return-type UtXdgPopup
UtObject *ut_xdg_surface_get_popup(UtObject *object, UtObject *parent,
                                   UtObject *positioner,
                                   UtObject *callback_object,
                                   const UtXdgPopupCallbacks *callbacks);

/// Sets the dimensions of this surface to [width]x[height] at [x],[y].
void ut_xdg_surface_set_window_geometry(UtObject *object, int32_t x, int32_t y,
                                        int32_t width, int32_t height);

/// Acknoledges a receive configure event with [serial].
void ut_xdg_surface_ack_configure(UtObject *object, uint32_t serial);

/// Returns [true] if [object] is a [UtXdgSurface].
bool ut_object_is_xdg_surface(UtObject *object);
