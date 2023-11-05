#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"
#include "ut-wayland-linux-buffer-params.h"

#pragma once

/// Creates a new Wayland DRM object with [id] on [client].
///
/// !arg-type client UtWaylandClient
/// !return-ref
/// !return-type UtWaylandLinuxDmabuf
UtObject *ut_wayland_linux_dmabuf_new(UtObject *client, uint32_t id);

/// Creates a new Wayland DRM object with well known [name] from
/// [registry].
///
/// !arg-type registry UtWaylandRegistry
/// !return-ref
/// !return-type UtWaylandLinuxDmabuf
UtObject *ut_wayland_linux_dmabuf_new_from_registry(UtObject *registry,
                                                    uint32_t name);

/// Returns the supported image formats.
///
/// !return-type UtUint32List
UtObject *ut_wayland_linux_dmabuf_get_formats(UtObject *object);

/// Returns the supported modifiers.
///
/// !return-type UtUint32List
UtObject *ut_wayland_linux_dmabuf_get_modifiers(UtObject *object);

/// Destroy this object.
void ut_wayland_linux_dmabuf_destroy(UtObject *object);

/// Creates a temporary object for buffer parameters.
/// The functions in [callbacks] will be called for any events for this object.
///
/// !return-ref
/// !return-type UtWaylandLinuxBufferParams
UtObject *ut_wayland_linux_dmabuf_create_params(
    UtObject *object, UtObject *callback_object,
    const UtWaylandLinuxBufferParamsCallbacks *callbacks);

/// !return-ref
/// !return-type UtWaylandLinuxDmabufFeedback
// UtObject* ut_wayland_linux_dmabuf_get_default_feedback(UtObject *object);

/// !arg-type surface UtWaylandSurface
/// !return-ref
/// !return-type UtWaylandLinuxDmabufFeedback
// UtObject *ut_wayland_linux_dmabuf_get_surface_feedback(UtObject *object,
// UtObject *surface);

/// Returns [true] if [object] is a [UtWaylandLinuxDmabuf].
bool ut_object_is_wayland_linux_dmabuf(UtObject *object);
