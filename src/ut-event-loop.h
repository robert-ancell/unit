#include <stdbool.h>
#include <sys/time.h>

#include "ut-object.h"

#pragma once

typedef void (*UtEventLoopCallback)(UtObject *user_data);
typedef UtObject *(*UtThreadCallback)(UtObject *data);
typedef void (*UtThreadResultCallback)(UtObject *object, UtObject *result);

UtObject *ut_event_loop_add_delay(time_t seconds, UtObject *callback_object,
                                  UtEventLoopCallback callback);

UtObject *ut_event_loop_add_timer(time_t seconds, UtObject *callback_object,
                                  UtEventLoopCallback callback);

void ut_event_loop_cancel_timer(UtObject *timer);

UtObject *ut_event_loop_add_read_watch(UtObject *fd, UtObject *callback_object,
                                       UtEventLoopCallback callback);

UtObject *ut_event_loop_add_write_watch(UtObject *fd, UtObject *callback_object,
                                        UtEventLoopCallback callback);

void ut_event_loop_cancel_watch(UtObject *watch);

void ut_event_loop_add_worker_thread(UtThreadCallback thread_callback,
                                     UtObject *thread_data,
                                     UtObject *callback_object,
                                     UtThreadResultCallback result_callback);

void ut_event_loop_return(UtObject *object);

UtObject *ut_event_loop_run();
