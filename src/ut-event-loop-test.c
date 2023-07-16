#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ut.h"

static UtObject *timer = NULL;

static void delay2_cb(UtObject *object) { printf("delay 2s\n"); }

static void delay3_cb(UtObject *object) { printf("delay 3s\n"); }

static void delay5_cb(UtObject *object) {
  printf("delay 5s\n");
  ut_event_loop_cancel_timer(timer);
}

static void timer_cb(UtObject *object) { printf("timer\n"); }

static UtObject *thread_cb(UtObject *object) {
  sleep(2);
  return ut_string_new("Hello World");
}

static void thread_result_cb(UtObject *object, UtObject *result) {
  printf("Thread result: '%s'\n", ut_string_get_text(result));
}

static void stdin_cb(UtObject *object) {
  char buffer[1024];
  ssize_t n_read = read(0, buffer, 1024);
  printf("stdin - '%.*s'\n", (int)n_read, buffer);
}

int main(int argc, char **argv) {
  UtObjectRef dummy_object = ut_null_new();
  UtObjectRef delay2_timer =
      ut_event_loop_add_delay(2, dummy_object, delay2_cb);
  UtObjectRef delay5_timer =
      ut_event_loop_add_delay(5, dummy_object, delay5_cb);
  UtObjectRef delay3_timer =
      ut_event_loop_add_delay(3, dummy_object, delay3_cb);
  timer = ut_event_loop_add_timer(1, dummy_object, timer_cb);

  ut_event_loop_add_worker_thread(thread_cb, NULL, dummy_object,
                                  thread_result_cb);

  UtObjectRef fd = ut_file_descriptor_new(0);
  UtObjectRef stdin_watch =
      ut_event_loop_add_read_watch(fd, dummy_object, stdin_cb);

  ut_event_loop_run();

  ut_object_unref(timer);

  return 0;
}
