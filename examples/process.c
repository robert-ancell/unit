#include <stdio.h>

#include "ut.h"

static UtObject *process = NULL;

static void complete_cb(UtObject *object) {
  if (ut_process_get_terminated(process)) {
    printf("Process terminated with signal %d\n",
           ut_process_get_termination_signal(process));
  } else {
    printf("Process returned with status %d\n",
           ut_process_get_exit_status(process));
  }

  UtObjectRef return_code = ut_int32_new(0);
  ut_event_loop_return(return_code);
}

int main(int argc, char **argv) {
  UtObjectRef sleep_argv =
      ut_string_list_new_from_elements("/usr/bin/sleep", "5", NULL);
  process = ut_process_new(sleep_argv);

  ut_cstring_ref argv_string = ut_string_list_join(sleep_argv, " ");
  printf("Running child process %s...\n", argv_string);
  UtObjectRef dummy_object = ut_null_new();
  ut_process_run(process, dummy_object, complete_cb);

  UtObjectRef return_code = ut_event_loop_run();
  return ut_int32_get_value(return_code);
}
