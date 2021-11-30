#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new TrueType interpreter to run [instructions].
///
/// !arg-type instructions UtUint8List
/// !return-ref
/// !return-type UtTrueTypeInterpreter
UtObject *ut_true_type_interpreter_new(UtObject *instructions);

/// Interpret the instructions.
void ut_true_type_interpreter_run(UtObject *object);

/// Returns the first error that occurred while running or [NULL] if no error
/// occurred.
///
/// !return-type UtTrueTypeError NULL
UtObject *ut_true_type_interpreter_get_error(UtObject *object);

/// Returns [true] if [object] is a [UtTrueTypeInterpreter].
bool ut_object_is_true_type_interpreter(UtObject *object);
