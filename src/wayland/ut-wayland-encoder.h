#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new Wayland encoder.
///
/// !return-ref
/// !return-type UtWaylandEncoder
UtObject *ut_wayland_encoder_new();

/// Adds a uint [value] to this message.
void ut_wayland_encoder_append_uint(UtObject *object, uint32_t value);

/// Adds an int [value] to this message.
void ut_wayland_encoder_append_int(UtObject *object, int32_t value);

/// Adds a string [value] to this message.
void ut_wayland_encoder_append_string(UtObject *object, const char *value);

/// Adds a [wayland_object] to this message.
void ut_wayland_encoder_append_object(UtObject *object,
                                      UtObject *wayland_object);

/// Adds a [fd] to this message.
void ut_wayland_encoder_append_fd(UtObject *object, UtObject *fd);

/// Returns the encoded data.
///
/// !return-type UtUint8List
UtObject *ut_wayland_encoder_get_data(UtObject *object);

/// Returns [true] if [object] is a [UtWaylandEncoder].
bool ut_object_is_wayland_encoder(UtObject *object);
