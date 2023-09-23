#include <stdbool.h>
#include <sys/time.h>

#include "ut-object.h"

#pragma once

typedef void (*UtEventLoopCallback)(UtObject *object);
typedef UtObject *(*UtThreadCallback)(UtObject *data);
typedef void (*UtThreadResultCallback)(UtObject *object, UtObject *result);

/// Add a [callback] to be called after [seconds].
/// Returns a handle that can be used in [ut_event_loop_cancel_timer].
///
/// !return-type UtObject
UtObject *ut_event_loop_add_delay(time_t seconds, UtObject *callback_object,
                                  UtEventLoopCallback callback);

/// Add a [callback] to be called every [seconds].
/// Returns a handle that can be used in [ut_event_loop_cancel_timer].
///
/// !return-type UtObject
UtObject *ut_event_loop_add_timer(time_t seconds, UtObject *callback_object,
                                  UtEventLoopCallback callback);

/// Cancels a previously started [timer].
///
/// !arg-type timer UtObject
void ut_event_loop_cancel_timer(UtObject *timer);

/// Add a [callback] to be called when [fd] is able to be read from.
///
/// !arg-type fd UtFileDescriptor
/// !return-type UtObject
UtObject *ut_event_loop_add_read_watch(UtObject *fd, UtObject *callback_object,
                                       UtEventLoopCallback callback);

/// Add a [callback] to be called when [fd] is able to be written to.
///
/// !arg-type fd UtFileDescriptor
/// !return-type UtObject
UtObject *ut_event_loop_add_write_watch(UtObject *fd, UtObject *callback_object,
                                        UtEventLoopCallback callback);

/// Cancels a previously registered [watch].
///
/// !arg-type watch UtObject
void ut_event_loop_cancel_watch(UtObject *watch);

/// Starts a thread in [thread_callback].
/// [thread_data] is passed to the new thread.
/// When the thread completes, [result_callback] is called.
///
/// !arg-type thread-data UtObject NULL.
void ut_event_loop_add_worker_thread(UtThreadCallback thread_callback,
                                     UtObject *thread_data,
                                     UtObject *callback_object,
                                     UtThreadResultCallback result_callback);

/// Complete the event loop and return [return_value].
///
/// !arg-type object UtObject NULL
void ut_event_loop_return(UtObject *return_value);

/// Run the event loop.
/// This will return when [ut_event_loop_return] is called.
///
/// !return-ref
/// !return-type UtObject NULL
UtObject *ut_event_loop_run();
