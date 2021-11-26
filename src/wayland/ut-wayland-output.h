#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef void (*UtWaylandOutputGeometryCallback)(
    void *user_data, int32_t x, int32_t y, int32_t physical_width,
    int32_t physical_height, int32_t subpixel, const char *make,
    const char *model, int32_t transform);
typedef void (*UtWaylandOutputModeCallback)(void *user_data, uint32_t flags,
                                            int32_t width, int32_t height,
                                            int32_t refresh);
typedef void (*UtWaylandOutputDoneCallback)(void *user_data);
typedef void (*UtWaylandOutputScaleCallback)(void *user_data, int32_t scale);
typedef void (*UtWaylandOutputNameCallback)(void *user_data, const char *name);
typedef void (*UtWaylandOutputDescriptionCallback)(void *user_data,
                                                   const char *description);

typedef struct {
  UtWaylandOutputGeometryCallback geometry;
  UtWaylandOutputModeCallback mode;
  UtWaylandOutputDoneCallback done;
  UtWaylandOutputScaleCallback scale;
  UtWaylandOutputNameCallback name;
  UtWaylandOutputDescriptionCallback description;
} UtWaylandOutputCallbacks;

UtObject *ut_wayland_output_new(UtObject *client, uint32_t id,
                                const UtWaylandOutputCallbacks *callbacks,
                                void *user_data);

void ut_wayland_output_release(UtObject *object);

bool ut_object_is_wayland_output(UtObject *object);
