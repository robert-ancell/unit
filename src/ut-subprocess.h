#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Method called when a process completes.
typedef void (*UtSubprocessCompleteCallback)(UtObject *object);

/// Creates a new process object to run [argv].
///
/// !arg-type argv UtStringList
/// !return-ref
/// !return-type UtSubprocess
UtObject *ut_subprocess_new(UtObject *argv);

/// Run the process. When complete [callback] is called.
void ut_subprocess_run(UtObject *object, UtObject *callback_object,
                       UtSubprocessCompleteCallback callback);

/// Returns the status the process completed with. Returns 0 if process not
/// completed or exited with a signal.
int ut_subprocess_get_exit_status(UtObject *object);

/// Returns TRUE if this process was terminated due to a signal.
bool ut_subprocess_get_terminated(UtObject *object);

/// Returns the signal that terminated the process. Returns 0 if process not
/// completed or terminated normally.
int ut_subprocess_get_termination_signal(UtObject *object);

/// Returns [true] if [object] is a [UtSubprocess].
bool ut_object_is_process(UtObject *object);
