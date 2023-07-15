#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ut.h"

static void delay2_cb(void *user_data) { printf("delay 2s\n"); }

static void delay3_cb(void *user_data) { printf("delay 3s\n"); }

static void delay5_cb(void *user_data) {
  UtObject *cancel = user_data;
  printf("delay 5s\n");
  ut_cancel_activate(cancel);
}

static void timer_cb(void *user_data) { printf("timer\n"); }

static void *thread_cb(UtObject *object) {
  sleep(2);
  return ut_cstring_new("Hello World");
}

static void thread_result_cb(UtObject *object, void *result) {
  char *result_ = result;
  printf("Thread result: '%s'\n", result_);
  free(result_);
}

static void stdin_cb(void *user_data) {
  char buffer[1024];
  ssize_t n_read = read(0, buffer, 1024);
  printf("stdin - '%.*s'\n", (int)n_read, buffer);
}

int main(int argc, char **argv) {
  UtObjectRef timer_cancel = ut_cancel_new();
  ut_event_loop_add_delay(2, delay2_cb, NULL, NULL);
  ut_event_loop_add_delay(5, delay5_cb, timer_cancel, NULL);
  ut_event_loop_add_delay(3, delay3_cb, NULL, NULL);
  ut_event_loop_add_timer(1, timer_cb, NULL, timer_cancel);

  UtObjectRef dummy_object = ut_null_new();
  ut_event_loop_add_worker_thread(thread_cb, NULL, dummy_object,
                                  thread_result_cb);

  ut_event_loop_add_read_watch(0, stdin_cb, NULL, NULL);

  ut_event_loop_run();

  return 0;
}
