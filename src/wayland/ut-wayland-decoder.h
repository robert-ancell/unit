#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_wayland_decoder_new(UtObject *data);

uint32_t ut_wayland_decoder_get_uint(UtObject *object);

int32_t ut_wayland_decoder_get_int(UtObject *object);

double ut_wayland_decoder_get_fixed(UtObject *object);

char *ut_wayland_decoder_get_string(UtObject *object);

UtObject *ut_wayland_decoder_get_uint_array(UtObject *object);

/// Returns [true] if [object] is a [UtWaylandDecoder].
bool ut_object_is_wayland_decoder(UtObject *object);
