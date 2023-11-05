#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef void (*UtWaylandDrmAuthenticatedCallback)(UtObject *object);

typedef struct {
  UtWaylandDrmAuthenticatedCallback authenticated;
} UtWaylandDrmCallbacks;

/// Creates a new Wayland DRM object with [id] on [client].
/// The functions in [callbacks] will be called for any events for this object.
///
/// !arg-type client UtWaylandClient
/// !return-ref
/// !return-type UtWaylandDrm
UtObject *ut_wayland_drm_new(UtObject *client, uint32_t id,
                             UtObject *callback_object,
                             const UtWaylandDrmCallbacks *callbacks);

/// Creates a new Wayland DRM object with well known [name] from
/// [registry].
/// The functions in [callbacks] will be called for any events for this object.
///
/// !arg-type registry UtWaylandRegistry
/// !return-ref
/// !return-type UtWaylandDrm
UtObject *
ut_wayland_drm_new_from_registry(UtObject *registry, uint32_t name,
                                 UtObject *callback_object,
                                 const UtWaylandDrmCallbacks *callbacks);

/// Returns the DRM device to use.
const char *ut_wayland_drm_get_device(UtObject *object);

/// Returns the supported image formats.
///
/// !return-type UtUint32List
UtObject *ut_wayland_drm_get_formats(UtObject *object);

/// Returns the capabilities.
uint32_t ut_wayland_drm_get_capabilities(UtObject *object);

/// Authenticate the DRM connection with with magic [id] returned from DRM.
/// Must be done before buffers can be created.
void ut_wayland_drm_authenticate(UtObject *object, uint32_t id);

/// Create a new buffer of size [width]x[height] in [format] from the DRM buffer
/// with [name]. Each row is [stride] bytes.
///
/// !return-ref
/// !return-type UtWaylandBuffer
UtObject *
ut_wayland_drm_create_buffer(UtObject *object, uint32_t name, int32_t width,
                             int32_t height, uint32_t stride, uint32_t format,
                             UtObject *callback_object,
                             UtWaylandBufferReleaseCallback release_callback);

/// Returns [true] if [object] is a [UtWaylandDrm].
bool ut_object_is_wayland_drm(UtObject *object);
