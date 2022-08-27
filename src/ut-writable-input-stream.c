#include <assert.h>

#include "ut-cancel.h"
#include "ut-input-stream.h"
#include "ut-writable-input-stream.h"

typedef struct {
  UtObject object;
  UtWritableInputStreamReadingCallback reading_callback;
  void *reading_user_data;
  UtObject *reading_cancel;
  UtInputStreamDataCallback callback;
  UtInputStreamClosedCallback closed_callback;
  void *user_data;
  UtObject *cancel;
} UtWritableInputStream;

static void ut_writable_input_stream_cleanup(UtObject *object) {
  UtWritableInputStream *self = (UtWritableInputStream *)object;
  ut_object_unref(self->reading_cancel);
  ut_object_unref(self->cancel);
}

static void
ut_writable_input_stream_read(UtObject *object,
                              UtInputStreamDataCallback callback,
                              UtInputStreamClosedCallback closed_callback,
                              void *user_data, UtObject *cancel) {
  UtWritableInputStream *self = (UtWritableInputStream *)object;
  assert(callback != NULL);
  assert(self->callback == NULL);

  self->callback = callback;
  self->closed_callback = closed_callback;
  self->user_data = user_data;
  self->cancel = ut_object_ref(cancel);

  if (self->reading_callback != NULL &&
      !ut_cancel_is_active(self->reading_cancel)) {
    self->reading_callback(self->reading_user_data, object);
  }
}

static UtInputStreamInterface input_stream_interface = {
    .read = ut_writable_input_stream_read};

static UtObjectInterface object_interface = {
    .type_name = "UtWritableInputStream",
    .cleanup = ut_writable_input_stream_cleanup,
    .interfaces = {{&ut_input_stream_id, &input_stream_interface},
                   {NULL, NULL}}};

UtObject *ut_writable_input_stream_new() {
  return ut_object_new(sizeof(UtWritableInputStream), &object_interface);
}

void ut_writable_input_stream_set_reading_callback(
    UtObject *object, UtWritableInputStreamReadingCallback reading_callback,
    void *user_data, UtObject *cancel) {
  assert(ut_object_is_writable_input_stream(object));
  UtWritableInputStream *self = (UtWritableInputStream *)object;

  assert(reading_callback != NULL);
  assert(self->reading_callback == NULL);

  self->reading_callback = reading_callback;
  self->reading_user_data = user_data;
  self->reading_cancel = ut_object_ref(cancel);
}

bool ut_writable_input_stream_get_reading(UtObject *object) {
  assert(ut_object_is_writable_input_stream(object));
  UtWritableInputStream *self = (UtWritableInputStream *)object;

  return self->callback != NULL && !ut_cancel_is_active(self->cancel);
}

size_t ut_writable_input_stream_write(UtObject *object, UtObject *data,
                                      bool complete) {
  assert(ut_object_is_writable_input_stream(object));
  UtWritableInputStream *self = (UtWritableInputStream *)object;

  assert(self->callback != NULL);

  if (ut_cancel_is_active(self->cancel)) {
    return 0;
  }

  return self->callback(self->user_data, data, complete);
}

void ut_writable_input_stream_close(UtObject *object, UtObject *data) {
  assert(ut_object_is_writable_input_stream(object));
  UtWritableInputStream *self = (UtWritableInputStream *)object;

  assert(self->callback != NULL);

  if (!ut_cancel_is_active(self->cancel)) {
    self->closed_callback(self->user_data, data);
  }
}

bool ut_object_is_writable_input_stream(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
