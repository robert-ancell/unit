#include <assert.h>

#include "ut-cancel.h"
#include "ut-input-stream.h"
#include "ut-list-input-stream.h"
#include "ut-list.h"

typedef struct {
  UtObject object;
  UtObject *data;
  UtInputStreamDataCallback callback;
  UtInputStreamClosedCallback closed_callback;
  void *user_data;
  UtObject *cancel;
  bool active;
  size_t offset;
  bool in_callback;
} UtListInputStream;

static void feed_data(UtListInputStream *self) {
  size_t data_length = ut_list_get_length(self->data);

  while (!ut_cancel_is_active(self->cancel) && self->active &&
         self->offset < data_length) {
    self->in_callback = true;
    UtObjectRef data = self->offset == 0
                           ? ut_object_ref(self->data)
                           : ut_list_get_sublist(self->data, self->offset,
                                                 data_length - self->offset);
    self->offset += self->callback(self->user_data, data, true);
    self->in_callback = false;
  }
  assert(self->offset <= data_length);

  if (!ut_cancel_is_active(self->cancel)) {
    UtObjectRef data = self->offset == 0
                           ? ut_object_ref(self->data)
                           : ut_list_get_sublist(self->data, self->offset,
                                                 data_length - self->offset);
    self->closed_callback(self->user_data, data);
  }
}

static void
ut_list_input_stream_read(UtObject *object, UtInputStreamDataCallback callback,
                          UtInputStreamClosedCallback closed_callback,
                          void *user_data, UtObject *cancel) {
  UtListInputStream *self = (UtListInputStream *)object;
  assert(self->callback == NULL);

  self->callback = callback;
  self->closed_callback = closed_callback;
  self->user_data = user_data;
  self->cancel = ut_object_ref(cancel);

  self->active = true;
  feed_data(self);
}

static void ut_list_input_stream_set_active(UtObject *object, bool active) {
  UtListInputStream *self = (UtListInputStream *)object;

  active = active ? true : false;
  if (self->active == active) {
    return;
  }
  self->active = active;

  if (self->active && !self->in_callback) {
    feed_data(self);
  }
}

static void ut_list_input_stream_cleanup(UtObject *object) {
  UtListInputStream *self = (UtListInputStream *)object;
  ut_object_unref(self->data);
  ut_object_unref(self->cancel);
}

static UtInputStreamInterface input_stream_interface = {
    .read = ut_list_input_stream_read,
    .set_active = ut_list_input_stream_set_active};

static UtObjectInterface object_interface = {
    .type_name = "UtListInputStream",
    .cleanup = ut_list_input_stream_cleanup,
    .interfaces = {{&ut_input_stream_id, &input_stream_interface},
                   {NULL, NULL}}};

UtObject *ut_list_input_stream_new(UtObject *list) {
  assert(list != NULL);
  UtObject *object =
      ut_object_new(sizeof(UtListInputStream), &object_interface);
  UtListInputStream *self = (UtListInputStream *)object;
  self->data = ut_object_ref(list);
  return object;
}

bool ut_object_is_list_input_stream(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
