#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_wayland_encoder_new();

void ut_wayland_encoder_append_uint(UtObject *object, uint32_t value);

void ut_wayland_encoder_append_int(UtObject *object, int32_t value);

void ut_wayland_encoder_append_string(UtObject *object, const char *value);

void ut_wayland_encoder_append_object(UtObject *object,
                                      UtObject *wayland_object);

void ut_wayland_encoder_append_fd(UtObject *object, UtObject *fd);

UtObject *ut_wayland_encoder_get_data(UtObject *object);

bool ut_object_is_wayland_encoder(UtObject *object);
