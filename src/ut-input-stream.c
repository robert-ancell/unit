#include <assert.h>
#include <stdlib.h>

#include "ut.h"

int ut_input_stream_id = 0;

typedef struct {
  UtObject object;
  UtObject *callback_object;
  UtInputStreamCallback callback;
} ReadAllData;

static void read_all_data_cleanup(UtObject *object) {
  ReadAllData *self = (ReadAllData *)object;
  ut_object_weak_unref(&self->callback_object);
}

static UtObjectInterface read_all_data_object_interface = {
    .type_name = "ReadAllData", .cleanup = read_all_data_cleanup};

static UtObject *read_all_data_new(UtObject *callback_object,
                                   UtInputStreamCallback callback) {
  UtObject *object =
      ut_object_new(sizeof(ReadAllData), &read_all_data_object_interface);
  ReadAllData *self = (ReadAllData *)object;
  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callback = callback;
  return object;
}

static size_t read_all_cb(UtObject *object, UtObject *data, bool complete) {
  ReadAllData *d = (ReadAllData *)object;

  if (d->callback_object == NULL) {
    ut_object_unref((UtObject *)d);
    return 0;
  }

  if (ut_object_implements_error(data)) {
    if (d->callback_object != NULL) {
      d->callback(d->callback_object, data, true);
    }
    ut_object_unref((UtObject *)d);
    return 0;
  } else if (complete) {
    if (d->callback_object != NULL) {
      d->callback(d->callback_object, data, true);
    }
    ut_object_unref((UtObject *)d);
    return ut_list_get_length(data);
  } else {
    // Wait for all data.
    return 0;
  }
}

static size_t sync_cb(UtObject *object, UtObject *data, bool complete) {
  if (ut_object_implements_error(data)) {
    ut_list_append(object, data);
    return 0;
  } else if (complete) {
    ut_list_append_take(object, ut_list_copy(data));
    return ut_list_get_length(data);
  } else {
    // Wait for all data.
    return 0;
  }
}

void ut_input_stream_read(UtObject *object, UtObject *callback_object,
                          UtInputStreamCallback callback) {
  UtInputStreamInterface *stream_interface =
      ut_object_get_interface(object, &ut_input_stream_id);
  assert(stream_interface != NULL);
  stream_interface->read(object, callback_object, callback);
}

void ut_input_stream_read_all(UtObject *object, UtObject *callback_object,
                              UtInputStreamCallback callback) {
  UtObject *data = read_all_data_new(callback_object, callback);
  ut_input_stream_read(object, data, read_all_cb);
}

UtObject *ut_input_stream_read_sync(UtObject *object) {
  UtObjectRef results = ut_list_new();
  ut_input_stream_read(object, results, sync_cb);
  if (ut_list_get_length(results) != 1) {
    return ut_error_new("Sync call did not complete");
  }
  return ut_list_get_element(results, 0);
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
