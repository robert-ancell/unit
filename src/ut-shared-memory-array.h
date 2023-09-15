#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new shared memory array.
///
/// !return-ref
/// !return-type UtSharedMemoryArray
UtObject *ut_shared_memory_array_new(size_t length);

/// Creates a new shared memory array from [fd].
///
/// !arg-type fd UtFileDescriptor
/// !return-ref
/// !return-type UtSharedMemoryArray
UtObject *ut_shared_memory_array_new_from_fd(UtObject *fd);

/// Returns the file descriptor for this shared memory.
///
/// !return-type UtFileDescriptor
UtObject *ut_shared_memory_array_get_fd(UtObject *object);

/// Returns the address of the shared memory.
uint8_t *ut_shared_memory_array_get_data(UtObject *object);

/// Returns [true] if [object] is a [UtSharedMemoryArray].
bool ut_object_is_shared_memory_array(UtObject *object);
