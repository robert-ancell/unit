#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>

#include "ut.h"

typedef struct _Timeout Timeout;
struct _Timeout {
  struct timespec when;
  struct timespec frequency;
  UtEventLoopCallback callback;
  void *user_data;
  UtObject *cancel;
  Timeout *next;
};

typedef struct _FdWatch FdWatch;
struct _FdWatch {
  UtObject *fd;
  UtEventLoopCallback callback;
  void *user_data;
  UtObject *cancel;
  FdWatch *next;
};

typedef struct {
  UtObject object;
  pthread_t thread_id;
  int complete_write_fd;
  int complete_read_fd;
  UtThreadCallback thread_callback;
  UtObject *thread_data;
  UtObject *callback_object;
  UtThreadResultCallback result_callback;
} WorkerThread;

typedef struct {
  UtObject object;
  Timeout *timeouts;
  FdWatch *read_watches;
  FdWatch *write_watches;
  UtObject *worker_threads;
  bool complete;
  UtObject *return_value;
} EventLoop;

static UtObject *loop = NULL;

static int time_compare(struct timespec *a, struct timespec *b) {
  if (a->tv_sec == b->tv_sec) {
    if (a->tv_nsec == b->tv_nsec) {
      return 0;
    }

    return a->tv_nsec > b->tv_nsec ? 1 : -1;
  }

  return a->tv_sec > b->tv_sec ? 1 : -1;
}

static void time_delta(struct timespec *a, struct timespec *b,
                       struct timespec *delta) {
  delta->tv_sec = b->tv_sec - a->tv_sec;
  if (b->tv_nsec > a->tv_nsec) {
    delta->tv_nsec = b->tv_nsec - a->tv_nsec;
  } else {
    delta->tv_sec--;
    delta->tv_nsec = 1000000000 + b->tv_nsec - a->tv_nsec;
  }
}

static void free_timeout(Timeout *timeout) {
  ut_object_unref(timeout->cancel);
  free(timeout);
}

static void insert_timeout(EventLoop *loop, Timeout *timeout) {
  Timeout *prev_timeout = NULL;
  for (Timeout *next_timeout = loop->timeouts; next_timeout != NULL;
       next_timeout = next_timeout->next) {
    if (time_compare(&next_timeout->when, &timeout->when) > 0) {
      break;
    }
    prev_timeout = next_timeout;
  }
  if (prev_timeout != NULL) {
    timeout->next = prev_timeout->next;
    prev_timeout->next = timeout;
  } else {
    timeout->next = loop->timeouts;
    loop->timeouts = timeout;
  }
}

static void add_timeout(EventLoop *loop, time_t seconds, bool repeat,
                        UtEventLoopCallback callback, void *user_data,
                        UtObject *cancel) {
  Timeout *t = malloc(sizeof(Timeout));
  assert(clock_gettime(CLOCK_MONOTONIC, &t->when) == 0);
  t->when.tv_sec += seconds;
  t->frequency.tv_sec = repeat ? seconds : 0;
  t->frequency.tv_nsec = 0;
  t->callback = callback;
  t->user_data = user_data;
  t->cancel = ut_object_ref(cancel);
  t->next = NULL;

  insert_timeout(loop, t);
}

static FdWatch *fd_watch_new(UtObject *fd, UtEventLoopCallback callback,
                             void *user_data, UtObject *cancel) {
  FdWatch *watch = malloc(sizeof(FdWatch));
  watch->fd = ut_object_ref(fd);
  watch->callback = callback;
  watch->user_data = user_data;
  watch->cancel = ut_object_ref(cancel);
  watch->next = NULL;
  return watch;
}

static void free_fd_watch(FdWatch *watch) {
  ut_object_unref(watch->fd);
  ut_object_unref(watch->cancel);
  free(watch);
}

static FdWatch *remove_cancelled_watches(FdWatch *watches) {
  FdWatch *prev_watch = NULL, *next_watch;
  for (FdWatch *watch = watches; watch != NULL; watch = next_watch) {
    next_watch = watch->next;
    if (ut_cancel_is_active(watch->cancel)) {
      if (prev_watch != NULL) {
        prev_watch->next = watch->next;
      } else {
        watches = watch->next;
      }
      watch->next = NULL;
      free_fd_watch(watch);
    } else {
      prev_watch = watch;
    }
  }

  return watches;
}

static void worker_thread_cleanup(UtObject *object) {
  WorkerThread *thread = (WorkerThread *)object;
  close(thread->complete_write_fd);
  close(thread->complete_read_fd);
  ut_object_unref(thread->thread_data);
  ut_object_weak_unref(&thread->callback_object);
}

static UtObjectInterface worker_thread_object_interface = {
    .type_name = "WorkerThread", .cleanup = worker_thread_cleanup};

static UtObject *worker_thread_new(UtThreadCallback thread_callback,
                                   void *thread_data, UtObject *callback_object,
                                   UtThreadResultCallback result_callback) {
  UtObject *object =
      ut_object_new(sizeof(WorkerThread), &worker_thread_object_interface);
  WorkerThread *self = (WorkerThread *)object;
  int fds[2];
  assert(pipe(fds) == 0);
  self->complete_write_fd = fds[1];
  self->complete_read_fd = fds[0];
  self->thread_callback = thread_callback;
  self->thread_data = thread_data;
  ut_object_weak_ref(callback_object, &self->callback_object);
  self->result_callback = result_callback;
  return object;
}

static void event_loop_init(UtObject *object) {
  EventLoop *self = (EventLoop *)object;
  self->worker_threads = ut_list_new();
}

static void event_loop_cleanup(UtObject *object) {
  EventLoop *self = (EventLoop *)object;
  ut_object_unref(self->worker_threads);
  ut_object_unref(self->return_value);
}

static UtObjectInterface event_loop_object_interface = {
    .type_name = "EventLoop",
    .init = event_loop_init,
    .cleanup = event_loop_cleanup};

static EventLoop *get_loop() {
  // FIXME: Check if this loop is for another thread, and make a new loop if so.
  if (loop == NULL) {
    loop = ut_object_new(sizeof(EventLoop), &event_loop_object_interface);
  }
  return (EventLoop *)loop;
}

void ut_event_loop_add_delay(time_t seconds, UtEventLoopCallback callback,
                             void *user_data, UtObject *cancel) {
  EventLoop *loop = get_loop();
  add_timeout(loop, seconds, false, callback, user_data, cancel);
}

void ut_event_loop_add_timer(time_t seconds, UtEventLoopCallback callback,
                             void *user_data, UtObject *cancel) {
  EventLoop *loop = get_loop();
  add_timeout(loop, seconds, true, callback, user_data, cancel);
}

void ut_event_loop_add_read_watch(UtObject *fd, UtEventLoopCallback callback,
                                  void *user_data, UtObject *cancel) {
  EventLoop *loop = get_loop();
  FdWatch *watch = fd_watch_new(fd, callback, user_data, cancel);
  watch->next = loop->read_watches;
  loop->read_watches = watch;
}

void ut_event_loop_add_write_watch(UtObject *fd, UtEventLoopCallback callback,
                                   void *user_data, UtObject *cancel) {
  EventLoop *loop = get_loop();
  FdWatch *watch = fd_watch_new(fd, callback, user_data, cancel);
  watch->next = loop->write_watches;
  loop->write_watches = watch;
}

static void *thread_cb(void *data) {
  WorkerThread *thread = data;

  void *result = thread->thread_callback(thread->thread_data);

  // Notify the main loop.
  uint8_t complete_data = 0;
  assert(write(thread->complete_write_fd, &complete_data, 1) == 1);

  return result;
}

void ut_event_loop_add_worker_thread(UtThreadCallback thread_callback,
                                     UtObject *thread_data,
                                     UtObject *callback_object,
                                     UtThreadResultCallback result_callback) {
  EventLoop *loop = get_loop();
  UtObjectRef thread_object = worker_thread_new(
      thread_callback, thread_data, callback_object, result_callback);
  ut_list_prepend(loop->worker_threads, thread_object);

  WorkerThread *thread = (WorkerThread *)thread_object;
  assert(pthread_create(&thread->thread_id, NULL, thread_cb, thread) == 0);
}

void ut_event_loop_return(UtObject *object) {
  EventLoop *loop = get_loop();
  assert(!loop->complete);
  loop->return_value = ut_object_ref(object);
  loop->complete = true;
}

UtObject *ut_event_loop_run() {
  EventLoop *self = get_loop();
  while (!self->complete) {
    // Do callbacks for any timers that have expired and work out time to next
    // timer.
    const struct timespec *timeout = NULL;
    struct timespec first_timeout;
    while (self->timeouts != NULL && timeout == NULL) {
      Timeout *t = self->timeouts;
      struct timespec now;
      assert(clock_gettime(CLOCK_MONOTONIC, &now) == 0);

      bool is_cancelled = ut_cancel_is_active(t->cancel);
      if (is_cancelled || time_compare(&t->when, &now) <= 0) {
        if (!is_cancelled) {
          t->callback(t->user_data);
        }

        self->timeouts = t->next;
        t->next = NULL;

        bool repeats = t->frequency.tv_sec != 0 || t->frequency.tv_nsec != 0;
        if (is_cancelled || !repeats) {
          free_timeout(t);
        } else {
          t->when.tv_sec += t->frequency.tv_sec;
          t->when.tv_nsec += t->frequency.tv_nsec;
          if (t->when.tv_nsec > 1000000000) {
            t->when.tv_sec++;
            t->when.tv_nsec -= 1000000000;
          }
          insert_timeout(self, t);
        }
      } else {
        time_delta(&now, &t->when, &first_timeout);
        timeout = &first_timeout;
      }
    }

    if (self->complete) {
      break;
    }

    int max_fd = -1;
    fd_set read_fds;
    fd_set write_fds;
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);

    // Listen for thread completion.
    size_t worker_threads_length = ut_list_get_length(self->worker_threads);
    for (size_t i = 0; i < worker_threads_length; i++) {
      WorkerThread *thread =
          (WorkerThread *)ut_object_list_get_element(self->worker_threads, i);
      FD_SET(thread->complete_read_fd, &read_fds);
      max_fd =
          thread->complete_read_fd > max_fd ? thread->complete_read_fd : max_fd;
    }

    // Register file descriptors we are watching for.
    self->read_watches = remove_cancelled_watches(self->read_watches);
    for (FdWatch *watch = self->read_watches; watch != NULL;
         watch = watch->next) {
      int fd = ut_file_descriptor_get_fd(watch->fd);
      FD_SET(fd, &read_fds);
      max_fd = fd > max_fd ? fd : max_fd;
    }
    self->write_watches = remove_cancelled_watches(self->write_watches);
    for (FdWatch *watch = self->write_watches; watch != NULL;
         watch = watch->next) {
      int fd = ut_file_descriptor_get_fd(watch->fd);
      FD_SET(fd, &write_fds);
      max_fd = fd > max_fd ? fd : max_fd;
    }

    // Wait for file descriptors or timeout.
    assert(pselect(max_fd + 1, &read_fds, &write_fds, NULL, timeout, NULL) >=
           0);

    // Complete any worker threads.
    worker_threads_length = ut_list_get_length(self->worker_threads);
    for (size_t i = 0; i < worker_threads_length;) {
      WorkerThread *thread =
          (WorkerThread *)ut_object_list_get_element(self->worker_threads, i);
      if (FD_ISSET(thread->complete_read_fd, &read_fds)) {
        void *r;
        assert(pthread_join(thread->thread_id, &r) == 0);
        UtObjectRef result = r;
        if (thread->callback_object != NULL &&
            thread->result_callback != NULL) {
          thread->result_callback(thread->callback_object, result);
        }
        ut_list_remove(self->worker_threads, i, 1);
        worker_threads_length--;
      } else {
        i++;
      }
    }

    // Do callbacks for each fd that has changed.
    // Note they are checked for cancellation as they might be cancelled in
    // these callbacks.
    for (FdWatch *watch = self->read_watches; watch != NULL;
         watch = watch->next) {
      if (!ut_cancel_is_active(watch->cancel) &&
          FD_ISSET(ut_file_descriptor_get_fd(watch->fd), &read_fds)) {
        watch->callback(watch->user_data);
      }
    }
    for (FdWatch *watch = self->write_watches; watch != NULL;
         watch = watch->next) {
      if (!ut_cancel_is_active(watch->cancel) &&
          FD_ISSET(ut_file_descriptor_get_fd(watch->fd), &write_fds)) {
        watch->callback(watch->user_data);
      }
    }

    // Purge any watches that have been removed.
    self->read_watches = remove_cancelled_watches(self->read_watches);
    self->write_watches = remove_cancelled_watches(self->write_watches);
  }

  UtObjectRef return_value = ut_object_ref(self->return_value);

  Timeout *next_timeout;
  for (Timeout *timeout = self->timeouts; timeout != NULL;
       timeout = next_timeout) {
    next_timeout = timeout->next;
    free_timeout(timeout);
  }
  FdWatch *next_watch;
  for (FdWatch *watch = self->read_watches; watch != NULL; watch = next_watch) {
    next_watch = watch->next;
    free_fd_watch(watch);
  }
  for (FdWatch *watch = self->write_watches; watch != NULL;
       watch = next_watch) {
    next_watch = watch->next;
    free_fd_watch(watch);
  }

  ut_object_clear(&loop);

  return ut_object_ref(return_value);
}
