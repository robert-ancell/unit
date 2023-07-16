#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtInputStreamCallback callback;
  void *user_data;
  UtObject *buffer;
  bool closed;
  bool complete;
} UtBufferedInputStream;

static void send_buffer(UtBufferedInputStream *self) {
  size_t buffer_length = ut_list_get_length(self->buffer);
  size_t n_used = self->callback(self->user_data, self->buffer, self->complete);
  if (n_used < buffer_length) {
    ut_list_remove(self->buffer, 0, n_used);
  } else {
    ut_object_unref(self->buffer);
    self->buffer = NULL;
  }
}

static void ut_buffered_input_stream_cleanup(UtObject *object) {
  UtBufferedInputStream *self = (UtBufferedInputStream *)object;
  ut_object_unref(self->buffer);
}

static void ut_buffered_input_stream_read(UtObject *object,
                                          UtInputStreamCallback callback,
                                          void *user_data) {
  UtBufferedInputStream *self = (UtBufferedInputStream *)object;
  assert(callback != NULL);
  assert(self->callback == NULL);
  assert(!self->closed);

  self->callback = callback;
  self->user_data = user_data;

  if (self->buffer != NULL) {
    send_buffer(self);
  }
}

static void ut_buffered_input_stream_close(UtObject *object) {
  UtBufferedInputStream *self = (UtBufferedInputStream *)object;
  self->closed = true;
}

static UtInputStreamInterface input_stream_interface = {
    .read = ut_buffered_input_stream_read,
    .close = ut_buffered_input_stream_close};

static UtObjectInterface object_interface = {
    .type_name = "UtBufferedInputStream",
    .cleanup = ut_buffered_input_stream_cleanup,
    .interfaces = {{&ut_input_stream_id, &input_stream_interface},
                   {NULL, NULL}}};

UtObject *ut_buffered_input_stream_new() {
  return ut_object_new(sizeof(UtBufferedInputStream), &object_interface);
}

void ut_buffered_input_stream_write(UtObject *object, UtObject *data,
                                    bool complete) {
  assert(ut_object_is_buffered_input_stream(object));
  UtBufferedInputStream *self = (UtBufferedInputStream *)object;

  assert(!self->closed);
  assert(!self->complete);
  self->complete = complete;

  // Not yet being read, so just buffer.
  if (self->callback == NULL) {
    if (self->buffer == NULL) {
      self->buffer = ut_list_copy(data);
    } else {
      ut_list_append_list(self->buffer, data);
    }
    return;
  }

  if (self->buffer != NULL) {
    ut_list_append_list(self->buffer, data);
    send_buffer(self);
  } else {
    size_t data_length = ut_list_get_length(data);
    size_t n_used = self->callback(self->user_data, data, complete);
    if (n_used < data_length) {
      UtObjectRef unused =
          ut_list_get_sublist(data, n_used, data_length - n_used);
      self->buffer = ut_list_copy(unused);
    }
  }
}

bool ut_object_is_buffered_input_stream(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
