#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new Wayland decoder to decode [data].
///
/// !arg-type data UtUint8List
/// !return-ref
/// !return-type UtWaylandDecoder
UtObject *ut_wayland_decoder_new(UtObject *data);

/// Returns the data used in this decoder.
///
/// !return-type UtUint8List
UtObject *ut_wayland_decoder_get_data(UtObject *object);

/// Returns a uint value read from this decoder.
uint32_t ut_wayland_decoder_get_uint(UtObject *object);

/// Returns an int value read from this decoder.
int32_t ut_wayland_decoder_get_int(UtObject *object);

/// Returns a fixed value read from this decoder.
double ut_wayland_decoder_get_fixed(UtObject *object);

/// Returns a string read from this decoder.
char *ut_wayland_decoder_get_string(UtObject *object);

/// Returns a uint array read from this decoder.
///
/// !return-ref
/// !return-type UtUint32List
UtObject *ut_wayland_decoder_get_uint_array(UtObject *object);

/// Returns [true] if [object] is a [UtWaylandDecoder].
bool ut_object_is_wayland_decoder(UtObject *object);
