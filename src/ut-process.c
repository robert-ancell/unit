#include <assert.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ut.h"

typedef struct {
  UtObject object;

  // Arguments to pass to process.
  UtObject *argv;

  // Method called when process completes.
  UtObject *callback_object;
  UtProcessCompleteCallback callback;

  // Pipe to send back return status.
  int status_pipe[2];

  // Watch to notify when status written.
  UtObject *status_watch;

  // Process ID of child.
  pid_t pid;

  // Status returned.
  int status;
} UtProcess;

static void status_cb(UtObject *object) {
  UtProcess *self = (UtProcess *)object;

  read(self->status_pipe[1], &self->status,
       sizeof(self->status)); // FIXME: Handle error
  // FIXME: Use status

  if (self->callback_object != NULL && self->callback != NULL) {
    self->callback(self->callback_object);
  }
}

static char *ut_process_to_string(UtObject *object) {
  UtProcess *self = (UtProcess *)object;
  ut_cstring_ref argv_string = ut_object_to_string(self->argv);
  return ut_cstring_new_printf("<UtProcess>(%s)", argv_string);
}

static void ut_process_cleanup(UtObject *object) {
  UtProcess *self = (UtProcess *)object;
  ut_object_unref(self->argv);
  ut_object_weak_unref(&self->callback_object);
  close(self->status_pipe[0]);
  close(self->status_pipe[1]);
  ut_object_unref(self->status_watch);
}

static UtObjectInterface object_interface = {.type_name = "UtProcess",
                                             .to_string = ut_process_to_string,
                                             .cleanup = ut_process_cleanup};

UtObject *ut_process_new(UtObject *argv) {
  assert(ut_list_get_length(argv) > 0);
  UtObject *object = ut_object_new(sizeof(UtProcess), &object_interface);
  UtProcess *self = (UtProcess *)object;
  self->argv = ut_object_ref(argv);
  return object;
}

void ut_process_run(UtObject *object, UtObject *callback_object,
                    UtProcessCompleteCallback callback) {
  assert(ut_object_is_process(object));
  UtProcess *self = (UtProcess *)object;

  assert(self->pid == 0);

  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callback = callback;

  const char *path = ut_string_list_get_element(self->argv, 0);
  size_t argv_length = ut_list_get_length(self->argv);
  char **argv = malloc(sizeof(char *) * (argv_length + 1));
  for (size_t i = 0; i < argv_length; i++) {
    argv[i] = (char *)ut_string_list_get_element(self->argv, i);
  }
  argv[argv_length] = NULL;

  // Create pipe for child process to notify return status.
  pipe(self->status_pipe); // FIXME: Handle error
  UtObjectRef fd = ut_file_descriptor_new(self->status_pipe[0]);
  self->status_watch = ut_event_loop_add_read_watch(
      fd, object, status_cb); // FIXME: Add fd watch into event loop

  // Create child process to monitor process to be run.
  self->pid = fork();
  if (self->pid == 0) {
    // Close read end of status pipe.
    close(self->status_pipe[0]);

    // Run process.
    pid_t pid = fork();
    if (pid == 0) {
      execv(path, argv);
      _exit(0);
    }

    // Wait for return status then report to parent.
    int status;
    waitpid(pid, &status, 0); // FIXME: handle error
    write(self->status_pipe[1], &status, sizeof(status));
    _exit(0);
  }
  // Close write end of status pipe.
  close(self->status_pipe[1]);

  free(argv);
}

int ut_process_get_exit_status(UtObject *object) {
  assert(ut_object_is_process(object));
  UtProcess *self = (UtProcess *)object;
  return WIFEXITED(self->status) ? WEXITSTATUS(self->status) : 0;
}

bool ut_process_get_terminated(UtObject *object) {
  assert(ut_object_is_process(object));
  UtProcess *self = (UtProcess *)object;
  return WIFSIGNALED(self->status);
}

int ut_process_get_termination_signal(UtObject *object) {
  assert(ut_object_is_process(object));
  UtProcess *self = (UtProcess *)object;
  return WIFSIGNALED(self->status) ? WTERMSIG(self->status) : 0;
}

bool ut_object_is_process(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
