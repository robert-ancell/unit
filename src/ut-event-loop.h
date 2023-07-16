#include <stdbool.h>
#include <sys/time.h>

#include "ut-object.h"

#pragma once

typedef void (*UtEventLoopCallback)(void *user_data);
typedef UtObject *(*UtThreadCallback)(UtObject *data);
typedef void (*UtThreadResultCallback)(UtObject *object, UtObject *result);

UtObject *ut_event_loop_add_delay(time_t seconds, UtEventLoopCallback callback,
                                  void *user_data, UtObject *cancel);

UtObject *ut_event_loop_add_timer(time_t seconds, UtEventLoopCallback callback,
                                  void *user_data, UtObject *cancel);

void ut_event_loop_cancel_timer(UtObject *timer);

void ut_event_loop_add_read_watch(UtObject *fd, UtEventLoopCallback callback,
                                  void *user_data, UtObject *cancel);

void ut_event_loop_add_write_watch(UtObject *fd, UtEventLoopCallback callback,
                                   void *user_data, UtObject *cancel);

void ut_event_loop_add_worker_thread(UtThreadCallback thread_callback,
                                     UtObject *thread_data,
                                     UtObject *callback_object,
                                     UtThreadResultCallback result_callback);

void ut_event_loop_return(UtObject *object);

UtObject *ut_event_loop_run();
