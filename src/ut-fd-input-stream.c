#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "ut-fd-input-stream.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *fd;
  UtObject *read_buffer;
  bool complete;
  size_t block_size;
  UtObject *read_watch;
  UtObject *callback_object;
  UtInputStreamCallback callback;
} UtFdInputStream;

static void read_cb(UtObject *object) {
  UtFdInputStream *self = (UtFdInputStream *)object;

  // Make space to read a new block.
  size_t buffer_length = ut_list_get_length(self->read_buffer);
  ut_list_resize(self->read_buffer, buffer_length + self->block_size);

  // Read a block.
  ssize_t n_read =
      read(ut_file_descriptor_get_fd(self->fd),
           ut_uint8_list_get_writable_data(self->read_buffer) + buffer_length,
           self->block_size);
  assert(n_read >= 0);
  buffer_length += n_read;
  ut_list_resize(self->read_buffer, buffer_length);

  // No more data to read.
  if (n_read == 0) {
    self->complete = true;
  }

  size_t n_used = self->callback_object != NULL
                      ? self->callback(self->callback_object, self->read_buffer,
                                       self->complete)
                      : 0;
  assert(n_used <= buffer_length);
  ut_list_remove(self->read_buffer, 0, n_used);
}

static void ut_fd_input_stream_init(UtObject *object) {
  UtFdInputStream *self = (UtFdInputStream *)object;
  self->read_buffer = ut_uint8_array_new();
  self->block_size = 4096;
}

static void ut_fd_input_stream_cleanup(UtObject *object) {
  UtFdInputStream *self = (UtFdInputStream *)object;

  ut_object_unref(self->fd);
  ut_object_unref(self->read_buffer);
  ut_object_unref(self->read_watch);
  ut_object_weak_unref(&self->callback_object);
}

static void ut_fd_input_stream_read(UtObject *object, UtObject *callback_object,
                                    UtInputStreamCallback callback) {
  UtFdInputStream *self = (UtFdInputStream *)object;
  assert(callback != NULL);

  assert(self->callback == NULL);

  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callback = callback;

  self->read_watch = ut_event_loop_add_read_watch(self->fd, object, read_cb);
}

static void ut_fd_input_stream_close(UtObject *object) {
  UtFdInputStream *self = (UtFdInputStream *)object;
  ut_event_loop_cancel_watch(self->read_watch);
}

static UtInputStreamInterface input_stream_interface = {
    .read = ut_fd_input_stream_read, .close = ut_fd_input_stream_close};

static UtObjectInterface object_interface = {
    .type_name = "UtFdInputStream",
    .init = ut_fd_input_stream_init,
    .cleanup = ut_fd_input_stream_cleanup,
    .interfaces = {{&ut_input_stream_id, &input_stream_interface},
                   {NULL, NULL}}};

UtObject *ut_fd_input_stream_new(UtObject *fd) {
  UtObject *object = ut_object_new(sizeof(UtFdInputStream), &object_interface);
  UtFdInputStream *self = (UtFdInputStream *)object;
  self->fd = ut_object_ref(fd);
  return object;
}

bool ut_object_is_fd_input_stream(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
