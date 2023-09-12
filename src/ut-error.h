#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef struct {
  char *(*get_description)(UtObject *object);
} UtErrorInterface;

extern int ut_error_id;

/// Creates a new error with [description].
/// This creates a [UtGeneralError].
///
/// !return-ref
/// !return-type UtError
UtObject *ut_error_new(const char *description);

/// Creates a new error with [description].
/// [description] must be allocated for this call, e.g. with
/// [ut_cstring_new_printf]. This creates a [UtGeneralError].
///
/// !return-ref
/// !return-type UtError
UtObject *ut_error_new_take(char *description);

/// Returns a human readable description of this error.
/// When no longer required used [free] to release the memory.
char *ut_error_get_description(UtObject *object);

/// Returns [true] if [object] is a [UtError].
bool ut_object_implements_error(UtObject *object);
