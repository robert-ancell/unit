#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef void (*UtWaylandOutputGeometryCallback)(
    UtObject *object, int32_t x, int32_t y, int32_t physical_width,
    int32_t physical_height, int32_t subpixel, const char *make,
    const char *model, int32_t transform);
typedef void (*UtWaylandOutputModeCallback)(UtObject *object, uint32_t flags,
                                            int32_t width, int32_t height,
                                            int32_t refresh);
typedef void (*UtWaylandOutputDoneCallback)(UtObject *object);
typedef void (*UtWaylandOutputScaleCallback)(UtObject *object, int32_t scale);
typedef void (*UtWaylandOutputNameCallback)(UtObject *object, const char *name);
typedef void (*UtWaylandOutputDescriptionCallback)(UtObject *object,
                                                   const char *description);

typedef struct {
  UtWaylandOutputGeometryCallback geometry;
  UtWaylandOutputModeCallback mode;
  UtWaylandOutputDoneCallback done;
  UtWaylandOutputScaleCallback scale;
  UtWaylandOutputNameCallback name;
  UtWaylandOutputDescriptionCallback description;
} UtWaylandOutputCallbacks;

/// Creates a new Wayland output object.
///
/// !arg-type client UtWaylandClient
/// !return-ref
/// !return-type UtWaylandOutput
UtObject *ut_wayland_output_new(UtObject *client, uint32_t id,
                                UtObject *callback_object,
                                const UtWaylandOutputCallbacks *callbacks);

/// Release this object.
void ut_wayland_output_release(UtObject *object);

/// Returns [true] if [object] is a [UtWaylandOutput].
bool ut_object_is_wayland_output(UtObject *object);
