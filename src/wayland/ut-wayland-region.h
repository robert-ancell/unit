#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_wayland_region_new(UtObject *client, uint32_t id);

void ut_wayland_region_destroy(UtObject *object);

void ut_wayland_region_add(UtObject *object, int32_t x, int32_t y,
                           int32_t width, int32_t height);

void ut_wayland_region_subtract(UtObject *object, int32_t x, int32_t y,
                                int32_t width, int32_t height);

bool ut_object_is_wayland_region(UtObject *object);
