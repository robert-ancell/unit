#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "ut-fd-input-stream.h"
#include "ut-fd-output-stream.h"
#include "ut.h"

typedef struct {
  UtObject object;
  char *path;
  UtObject *fd;
  UtObject *error;
  UtObject *input_stream;
  UtObject *output_stream;
} UtLocalFile;

static void ut_local_file_cleanup(UtObject *object) {
  UtLocalFile *self = (UtLocalFile *)object;
  // FIXME: Cancel read/writes
  free(self->path);
  self->path = NULL;
  ut_object_unref(self->fd);
  ut_object_unref(self->error);
  ut_object_unref(self->input_stream);
  ut_object_unref(self->output_stream);
}

static void ut_local_file_open_read(UtObject *object) {
  UtLocalFile *self = (UtLocalFile *)object;
  assert(self->fd == NULL && self->error == NULL);
  int fd = open(self->path, O_RDONLY);
  if (fd < 0) {
    self->error = ut_error_new("Failed to open file for reading");
    return;
  }
  self->fd = ut_file_descriptor_new(fd);
  self->input_stream = ut_fd_input_stream_new(self->fd);
}

static void ut_local_file_open_write(UtObject *object, bool create) {
  UtLocalFile *self = (UtLocalFile *)object;
  assert(self->fd == NULL);
  int flags = O_TRUNC;
  if (create) {
    flags |= O_CREAT;
  }
  int fd = open(self->path, O_WRONLY | flags,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
  if (fd < 0) {
    self->error = ut_error_new("Failed to open file for writing");
    return;
  }
  self->fd = ut_file_descriptor_new(fd);
  self->output_stream = ut_fd_output_stream_new(self->fd);
}

static void ut_local_file_close(UtObject *object) {
  UtLocalFile *self = (UtLocalFile *)object;
  if (self->fd != NULL) {
    ut_file_descriptor_close(self->fd);
  }
}

static void ut_local_file_read(UtObject *object, UtObject *callback_object,
                               UtInputStreamCallback callback) {
  UtLocalFile *self = (UtLocalFile *)object;

  if (self->error != NULL) {
    callback(callback_object, self->error, true);
    return;
  }

  assert(self->input_stream != NULL);

  ut_input_stream_read(self->input_stream, callback_object, callback);
}

static void ut_local_file_close_stream(UtObject *object) {
  UtLocalFile *self = (UtLocalFile *)object;

  assert(self->input_stream != NULL);
  ut_input_stream_close(self->input_stream);
}

static void ut_local_file_write(UtObject *object, UtObject *data,
                                UtObject *callback_object,
                                UtOutputStreamCallback callback) {
  UtLocalFile *self = (UtLocalFile *)object;

  if (self->error != NULL) {
    // FIXME: Report error?
    return;
  }

  assert(self->output_stream != NULL);

  ut_output_stream_write_full(self->output_stream, data, callback_object,
                              callback);
}

static UtFileInterface file_interface = {.open_read = ut_local_file_open_read,
                                         .open_write = ut_local_file_open_write,
                                         .close = ut_local_file_close};

static UtInputStreamInterface input_stream_interface = {
    .read = ut_local_file_read, .close = ut_local_file_close_stream};

static UtOutputStreamInterface output_stream_interface = {
    .write = ut_local_file_write};

static UtObjectInterface object_interface = {
    .type_name = "UtLocalFile",
    .cleanup = ut_local_file_cleanup,
    .interfaces = {{&ut_file_id, &file_interface},
                   {&ut_input_stream_id, &input_stream_interface},
                   {&ut_output_stream_id, &output_stream_interface},
                   {NULL, NULL}}};

UtObject *ut_local_file_new(const char *path) {
  UtObject *object = ut_object_new(sizeof(UtLocalFile), &object_interface);
  UtLocalFile *self = (UtLocalFile *)object;
  self->path = ut_cstring_new(path);
  return object;
}

UtObject *ut_local_file_get_fd(UtObject *object) {
  assert(ut_object_is_local_file(object));
  UtLocalFile *self = (UtLocalFile *)object;
  return self->fd;
}

bool ut_object_is_local_file(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
