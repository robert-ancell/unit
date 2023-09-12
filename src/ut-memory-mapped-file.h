#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_memory_mapped_file_new(const char *path);

void *ut_memory_mapped_file_open();

uint8_t *ut_memory_mapped_file_get_data(UtObject *object);

/// Returns [true] if [object] is a [UtMemoryMappedFile].
bool ut_object_is_memory_mapped_file(UtObject *object);
