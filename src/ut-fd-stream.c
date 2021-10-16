#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ut-cancel.h"
#include "ut-event-loop.h"
#include "ut-fd-stream.h"
#include "ut-list.h"
#include "ut-mutable-list.h"
#include "ut-mutable-uint8-list.h"
#include "ut-object-private.h"
#include "ut-uint8-list.h"

typedef struct {
  UtObject object;
  int fd;
  UtObject *read_buffer;
  size_t read_buffer_length;
} UtFdStream;

typedef enum { READ_MODE_SINGLE, READ_MODE_STREAM, READ_MODE_ALL } ReadMode;

typedef struct {
  UtFdStream *self;
  size_t block_length;
  ReadMode mode;
  UtObject *watch_cancel;
  UtFdStreamReadCallback callback;
  void *user_data;
  UtObject *cancel;
} ReadData;

typedef struct {
  UtFdStream *self;
  UtObject *data;
  size_t n_written;
  bool write_all;
  UtObject *watch_cancel;
  UtFdStreamWriteCallback callback;
  void *user_data;
  UtObject *cancel;
} WriteData;

static ReadData *read_data_new(UtFdStream *self, size_t block_length,
                               ReadMode mode, UtFdStreamReadCallback callback,
                               void *user_data, UtObject *cancel) {
  ReadData *data = malloc(sizeof(ReadData));
  data->self = self;
  data->block_length = block_length;
  data->mode = mode;
  data->watch_cancel = ut_cancel_new();
  data->callback = callback;
  data->user_data = user_data;
  data->cancel = cancel != NULL ? ut_object_ref(cancel) : NULL;
  return data;
}

static void read_data_free(ReadData *data) {
  ut_object_unref(data->watch_cancel);
  if (data->cancel) {
    ut_object_unref(data->cancel);
  }
  free(data);
}

static void report_read_data(ReadData *data) {
  UtFdStream *self = data->self;

  // FIXME: Can report more data than requested in a single read - trim buffer
  // in this case.
  ut_mutable_list_resize(self->read_buffer, self->read_buffer_length);
  size_t n_used;
  if (data->callback != NULL) {
    n_used = data->callback(data->user_data, self->read_buffer);
  } else {
    n_used = self->read_buffer_length;
  }

  ut_mutable_list_remove(self->read_buffer, 0, n_used);
  self->read_buffer_length -= n_used;
}

static void read_cb(void *user_data) {
  ReadData *data = user_data;
  UtFdStream *self = data->self;

  bool done = false;
  if (data->cancel == NULL || !ut_cancel_is_active(data->cancel)) {
    // Make space to read a new block.
    ut_mutable_list_resize(self->read_buffer,
                           self->read_buffer_length + data->block_length);

    // Read a block.
    ssize_t n_read = read(self->fd,
                          ut_mutable_uint8_list_get_data(self->read_buffer) +
                              self->read_buffer_length,
                          data->block_length);
    assert(n_read >= 0);
    self->read_buffer_length += n_read;

    if (n_read == 0) {
      report_read_data(data);
      done = data->mode == READ_MODE_SINGLE || self->read_buffer_length == 0;
    } else if (data->mode != READ_MODE_ALL) {
      report_read_data(data);
      done = data->mode == READ_MODE_SINGLE;
    }
  } else {
    done = true;
  }

  // Stop listening for read events when done.
  if (done) {
    ut_cancel_activate(data->watch_cancel);
    read_data_free(data);
  }
}

static void add_read_watch(ReadData *data) {
  UtFdStream *self = data->self;
  ut_event_loop_add_read_watch(self->fd, read_cb, data, data->watch_cancel);
}

static void buffered_read_cb(void *user_data) {
  ReadData *data = user_data;
  if (data->cancel != NULL && ut_cancel_is_active(data->cancel)) {
    return;
  }
  report_read_data(data);
  if (data->mode == READ_MODE_SINGLE) {
    return;
  }
  if (data->cancel != NULL && ut_cancel_is_active(data->cancel)) {
    return;
  }
  add_read_watch(data);
}

static void start_read(UtFdStream *self, size_t block_length, ReadMode mode,
                       UtFdStreamReadCallback callback, void *user_data,
                       UtObject *cancel) {
  ReadData *data =
      read_data_new(self, block_length, mode, callback, user_data, cancel);

  // If have buffered data, process that first.
  if (self->read_buffer_length > 0) {
    ut_event_loop_add_delay(0, buffered_read_cb, data, cancel);
  } else {
    add_read_watch(data);
  }
}

static WriteData *write_data_new(UtFdStream *self, UtObject *data_,
                                 bool write_all,
                                 UtFdStreamWriteCallback callback,
                                 void *user_data, UtObject *cancel) {
  WriteData *data = malloc(sizeof(WriteData));
  data->self = self;
  data->data = ut_object_ref(data_);
  data->n_written = 0;
  data->write_all = write_all;
  data->watch_cancel = ut_cancel_new();
  data->callback = callback;
  data->user_data = user_data;
  data->cancel = cancel != NULL ? ut_object_ref(cancel) : NULL;
  return data;
}

static void write_data_free(WriteData *data) {
  ut_object_unref(data->data);
  ut_object_unref(data->watch_cancel);
  if (data->cancel) {
    ut_object_unref(data->cancel);
  }
  free(data);
}

static void write_cb(void *user_data) {
  WriteData *data = user_data;
  UtFdStream *self = data->self;

  bool done = false;
  if (data->cancel == NULL || !ut_cancel_is_active(data->cancel)) {
    // Write remaining data.
    ssize_t n_written =
        write(self->fd, ut_uint8_list_get_data(data->data) + data->n_written,
              ut_list_get_length(data->data) - data->n_written);
    assert(n_written >= 0);
    data->n_written += n_written;

    // Done if all data written or only doing single write.
    done =
        data->n_written == ut_list_get_length(data->data) || !data->write_all;

    // Report how much data was written.
    if (done && data->callback != NULL) {
      data->callback(data->user_data, data->n_written);
    }
  } else {
    done = true;
  }

  // Stop listening for read events when done.
  if (done) {
    ut_cancel_activate(data->watch_cancel);
    write_data_free(data);
  }
}

static void ut_fd_stream_init(UtObject *object) {
  UtFdStream *self = (UtFdStream *)object;
  self->fd = -1;
  self->read_buffer = ut_mutable_uint8_list_new();
  self->read_buffer_length = 0;
}

static void ut_fd_stream_cleanup(UtObject *object) {
  UtFdStream *self = (UtFdStream *)object;
  ut_object_unref(self->read_buffer);
}

static UtObjectFunctions object_functions = {.type_name = "FdStream",
                                             .init = ut_fd_stream_init,
                                             .cleanup = ut_fd_stream_cleanup};

UtObject *ut_fd_stream_new(int fd) {
  UtObject *object = ut_object_new(sizeof(UtFdStream), &object_functions);
  UtFdStream *self = (UtFdStream *)object;
  self->fd = fd;
  return object;
}

void ut_fd_stream_read(UtObject *object, size_t count,
                       UtFdStreamReadCallback callback, void *user_data,
                       UtObject *cancel) {
  UtFdStream *self = (UtFdStream *)object;
  assert(self->fd >= 0);

  start_read(self, count, READ_MODE_SINGLE, callback, user_data, cancel);
}

void ut_fd_stream_read_stream(UtObject *object, size_t block_size,
                              UtFdStreamReadCallback callback, void *user_data,
                              UtObject *cancel) {
  UtFdStream *self = (UtFdStream *)object;
  assert(self->fd >= 0);

  start_read(self, block_size, READ_MODE_STREAM, callback, user_data, cancel);
}

void ut_fd_stream_read_all(UtObject *object, size_t block_size,
                           UtFdStreamReadCallback callback, void *user_data,
                           UtObject *cancel) {
  UtFdStream *self = (UtFdStream *)object;
  assert(self->fd >= 0);

  start_read(self, block_size, READ_MODE_ALL, callback, user_data, cancel);
}

void ut_fd_stream_write(UtObject *object, UtObject *data,
                        UtFdStreamWriteCallback callback, void *user_data,
                        UtObject *cancel) {
  UtFdStream *self = (UtFdStream *)object;
  assert(self->fd >= 0);

  WriteData *callback_data =
      write_data_new(self, data, false, callback, user_data, cancel);
  ut_event_loop_add_write_watch(self->fd, write_cb, callback_data,
                                callback_data->watch_cancel);
}

void ut_fd_stream_write_all(UtObject *object, UtObject *data,
                            UtFdStreamWriteCallback callback, void *user_data,
                            UtObject *cancel) {
  UtFdStream *self = (UtFdStream *)object;
  assert(self->fd >= 0);

  WriteData *callback_data =
      write_data_new(self, data, true, callback, user_data, cancel);
  ut_event_loop_add_write_watch(self->fd, write_cb, callback_data,
                                callback_data->watch_cancel);
}

bool ut_object_is_fd_stream(UtObject *object) {
  return ut_object_is_type(object, &object_functions);
}
