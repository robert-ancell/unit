#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_xdg_positioner_new(UtObject *client, uint32_t id);

void ut_xdg_positioner_destroy(UtObject *object);

void ut_xdg_positioner_set_size(UtObject *object, int32_t width,
                                int32_t height);

void ut_xdg_positioner_set_anchor_rect(UtObject *object, int32_t x, int32_t y,
                                       int32_t width, int32_t height);

void ut_xdg_positioner_set_anchor(UtObject *object, uint32_t anchor);

void ut_xdg_positioner_set_gravity(UtObject *object, uint32_t gravity);

void ut_xdg_positioner_set_constraint_adjustment(
    UtObject *object, uint32_t constraint_adjustment);

void ut_xdg_positioner_set_offset(UtObject *object, int32_t x, int32_t y);

void ut_xdg_positioner_set_reactive(UtObject *object);

void ut_xdg_positioner_set_parent_size(UtObject *object, int32_t parent_width,
                                       int32_t parent_height);

void ut_xdg_positioner_set_parent_configure(UtObject *object, uint32_t serial);

bool ut_object_is_xdg_positioner(UtObject *object);
