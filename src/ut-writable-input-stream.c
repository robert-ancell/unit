#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *reading_callback_object;
  UtWritableInputStreamReadingCallback reading_callback;
  UtObject *callback_object;
  UtInputStreamCallback callback;
  bool closed;
} UtWritableInputStream;

static void ut_writable_input_stream_cleanup(UtObject *object) {
  UtWritableInputStream *self = (UtWritableInputStream *)object;
  ut_object_weak_unref(&self->reading_callback_object);
  ut_object_weak_unref(&self->callback_object);
}

static void ut_writable_input_stream_read(UtObject *object,
                                          UtObject *callback_object,
                                          UtInputStreamCallback callback) {
  UtWritableInputStream *self = (UtWritableInputStream *)object;
  assert(callback != NULL);
  assert(self->callback == NULL);
  assert(!self->closed);

  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callback = callback;

  if (self->reading_callback_object != NULL && self->reading_callback != NULL) {
    self->reading_callback(self->reading_callback_object, object);
  }
}

static void ut_writable_input_stream_close(UtObject *object) {
  UtWritableInputStream *self = (UtWritableInputStream *)object;

  self->closed = true;
}

static UtInputStreamInterface input_stream_interface = {
    .read = ut_writable_input_stream_read,
    .close = ut_writable_input_stream_close};

static UtObjectInterface object_interface = {
    .type_name = "UtWritableInputStream",
    .cleanup = ut_writable_input_stream_cleanup,
    .interfaces = {{&ut_input_stream_id, &input_stream_interface},
                   {NULL, NULL}}};

UtObject *ut_writable_input_stream_new() {
  return ut_object_new(sizeof(UtWritableInputStream), &object_interface);
}

void ut_writable_input_stream_set_reading_callback(
    UtObject *object, UtObject *callback_object,
    UtWritableInputStreamReadingCallback reading_callback) {
  assert(ut_object_is_writable_input_stream(object));
  UtWritableInputStream *self = (UtWritableInputStream *)object;

  assert(reading_callback != NULL);
  assert(self->reading_callback == NULL);

  ut_object_weak_ref(callback_object, &self->reading_callback_object);
  self->reading_callback = reading_callback;
}

size_t ut_writable_input_stream_write(UtObject *object, UtObject *data,
                                      bool complete) {
  assert(ut_object_is_writable_input_stream(object));
  UtWritableInputStream *self = (UtWritableInputStream *)object;

  if (self->closed) {
    return 0;
  }

  assert(self->callback != NULL);

  return self->callback_object != NULL
             ? self->callback(self->callback_object, data, complete)
             : 0;
}

bool ut_object_is_writable_input_stream(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
