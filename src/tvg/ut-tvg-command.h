#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef struct {
} UtTvgCommandInterface;

extern int ut_tvg_command_id;

/// Returns [true] if [object] implements [UtTvgCommand].
bool ut_object_implements_tvg_command(UtObject *object);
