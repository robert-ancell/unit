#include <assert.h>

#include "ut.h"

int ut_output_stream_id = 0;

void ut_output_stream_write(UtObject *object, UtObject *data) {
  ut_output_stream_write_full(object, data, NULL, NULL);
}

void ut_output_stream_write_full(UtObject *object, UtObject *data,
                                 UtObject *callback_object,
                                 UtOutputStreamCallback callback) {
  UtOutputStreamInterface *stream_interface =
      ut_object_get_interface(object, &ut_output_stream_id);
  assert(stream_interface != NULL);
  stream_interface->write(object, data, callback_object, callback);
}

bool ut_object_implements_output_stream(UtObject *object) {
  return ut_object_get_interface(object, &ut_output_stream_id) != NULL;
}
