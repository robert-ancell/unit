#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new list of unsigned 8 bit values from [data] with associated
/// [fds].
///
/// !arg-type data UtUint8List
/// !arg-type fds UtObjectList
/// !return-ref
/// !return-type UtUint8ArrayWithFds
UtObject *ut_uint8_array_with_fds_new(UtObject *data, UtObject *fds);

/// Returns the unsigned 8 bit data.
///
/// !return-type UtUint8List
UtObject *ut_uint8_array_with_fds_get_data(UtObject *object);

/// Returns the file descriptors.
///
/// !return-type UtObjectList
UtObject *ut_uint8_array_with_fds_get_fds(UtObject *object);

/// Returns [true] if [object] is a [UtUint8ArrayWithFds].
bool ut_object_is_uint8_array_with_fds(UtObject *object);
