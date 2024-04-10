#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef struct {
} UtTvgStyleInterface;

extern int ut_tvg_style_id;

/// Returns [true] if [object] implements [UtTvgStyle].
bool ut_object_implements_tvg_style(UtObject *object);
