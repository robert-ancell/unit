#include <stdarg.h>
#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef struct {
  char *(*get_description)(UtObject *object);
} UtErrorInterface;

extern int ut_error_id;

UtObject *ut_error_new(const char *format, ...);

/// Returns a human readable description of this error.
/// When no longer required used [free] to release the memory.
char *ut_error_get_description(UtObject *object);

/// Returns [true] if [object] is a [UtError].
bool ut_object_implements_error(UtObject *object);
