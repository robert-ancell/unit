#include <assert.h>
#include <stdlib.h>

#include "ut.h"

int ut_input_stream_id = 0;

typedef struct {
  UtObject object;
  UtInputStreamCallback callback;
  void *user_data;
} ReadAllData;

static UtObjectInterface read_all_data_object_interface = {.type_name =
                                                               "ReadAllData"};

static UtObject *read_all_data_new(UtInputStreamCallback callback,
                                   void *user_data) {
  UtObject *object =
      ut_object_new(sizeof(ReadAllData), &read_all_data_object_interface);
  ReadAllData *self = (ReadAllData *)object;
  self->callback = callback;
  self->user_data = user_data;
  return object;
}

static size_t read_all_cb(void *user_data, UtObject *data, bool complete) {
  ReadAllData *d = user_data;

  if (ut_object_implements_error(data)) {
    d->callback(d->user_data, data, true);
    ut_object_unref((UtObject *)d);
    return 0;
  } else if (complete) {
    d->callback(d->user_data, data, true);
    ut_object_unref((UtObject *)d);
    return ut_list_get_length(data);
  } else {
    // Wait for all data.
    return 0;
  }
}

static size_t sync_cb(void *user_data, UtObject *data, bool complete) {
  UtObject **result = user_data;
  if (ut_object_implements_error(data)) {
    *result = ut_object_ref(data);
    return 0;
  } else if (complete) {
    assert(*result == NULL);
    *result = ut_list_copy(data);
    return ut_list_get_length(data);
  } else {
    // Wait for all data.
    return 0;
  }
}

void ut_input_stream_read(UtObject *object, UtInputStreamCallback callback,
                          void *user_data) {
  UtInputStreamInterface *stream_interface =
      ut_object_get_interface(object, &ut_input_stream_id);
  assert(stream_interface != NULL);
  stream_interface->read(object, callback, user_data);
}

void ut_input_stream_read_all(UtObject *object, UtInputStreamCallback callback,
                              void *user_data) {
  UtObject *data = read_all_data_new(callback, user_data);
  ut_input_stream_read(object, read_all_cb, data);
}

UtObject *ut_input_stream_read_sync(UtObject *object) {
  UtObject *result = NULL;
  ut_input_stream_read(object, sync_cb, &result);
  if (result == NULL) {
    result = ut_general_error_new("Sync call did not complete");
  }
  return result;
}

void ut_input_stream_close(UtObject *object) {
  UtInputStreamInterface *stream_interface =
      ut_object_get_interface(object, &ut_input_stream_id);
  assert(stream_interface != NULL);
  if (stream_interface->close != NULL) {
    stream_interface->close(object);
  }
}

bool ut_object_implements_input_stream(UtObject *object) {
  return ut_object_get_interface(object, &ut_input_stream_id) != NULL;
}
