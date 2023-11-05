#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef void (*UtWaylandLinuxBufferParamsCreatedCallback)(UtObject *object,
                                                          uint32_t id);
typedef void (*UtWaylandLinuxBufferParamsFailedCallback)(UtObject *object);

typedef struct {
  UtWaylandLinuxBufferParamsCreatedCallback created;
  UtWaylandLinuxBufferParamsFailedCallback failed;
} UtWaylandLinuxBufferParamsCallbacks;

/// Creates a new Linux Buffer Params object with [id] on [client].
/// The functions in [callbacks] will be called for any events for this object.
///
/// !arg-type client UtWaylandClient
/// !return-ref
/// !return-type UtWaylandLinuxBufferParams
UtObject *ut_wayland_linux_buffer_params_new(
    UtObject *client, uint32_t id, UtObject *callback_object,
    const UtWaylandLinuxBufferParamsCallbacks *callbacks);

/// Destroy this object.
void ut_wayland_linux_buffer_params_destroy(UtObject *object);

/// !art-type fd UtFileDescriptor
void ut_wayland_linux_buffer_params_add(UtObject *object, UtObject *fd,
                                        uint32_t plane_index, uint32_t offset,
                                        uint32_t stride, uint64_t modifier);

void ut_wayland_linux_buffer_params_create(UtObject *object, int32_t width,
                                           int32_t height, uint32_t format,
                                           uint32_t flags);

/// Returns [true] if [object] is a [UtWaylandLinuxBufferParams].
bool ut_object_is_wayland_linux_buffer_params(UtObject *object);
