#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new memory mapped file that accesses [path].
///
/// !return-ref
/// !return-type UtMemoryMappedFile
UtObject *ut_memory_mapped_file_new(const char *path);

/// Returns the address of the memory mapped file or NULL if not yet
/// opened.
uint8_t *ut_memory_mapped_file_get_data(UtObject *object);

/// Returns [true] if [object] is a [UtMemoryMappedFile].
bool ut_object_is_memory_mapped_file(UtObject *object);
