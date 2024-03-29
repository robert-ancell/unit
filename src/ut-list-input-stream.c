#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *data;
  bool read;
  bool closed;
} UtListInputStream;

static void ut_list_input_stream_read(UtObject *object,
                                      UtObject *callback_object,
                                      UtInputStreamCallback callback) {
  UtListInputStream *self = (UtListInputStream *)object;

  assert(!self->read);
  assert(!self->closed);
  self->read = true;

  size_t data_length = ut_list_get_length(self->data);
  size_t n_used = callback(callback_object, self->data, true);
  assert(n_used <= data_length);
}

static void ut_list_input_stream_close(UtObject *object) {
  UtListInputStream *self = (UtListInputStream *)object;
  self->closed = true;
}

static void ut_list_input_stream_cleanup(UtObject *object) {
  UtListInputStream *self = (UtListInputStream *)object;
  ut_object_unref(self->data);
}

static UtInputStreamInterface input_stream_interface = {
    .read = ut_list_input_stream_read, .close = ut_list_input_stream_close};

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
