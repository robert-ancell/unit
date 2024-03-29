#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *input;
  UtObject *buffer;

  UtObject *callback_object;
  UtInputStreamCallback callback;
} UtUtf8Encoder;

static void ut_utf8_encoder_init(UtObject *object) {
  UtUtf8Encoder *self = (UtUtf8Encoder *)object;
  self->buffer = ut_uint8_list_new();
}

static void ut_utf8_encoder_cleanup(UtObject *object) {
  UtUtf8Encoder *self = (UtUtf8Encoder *)object;
  ut_object_unref(self->input);
  ut_object_unref(self->buffer);
  ut_object_weak_unref(&self->callback_object);
}

static size_t read_cb(UtObject *object, UtObject *data, bool complete) {
  UtUtf8Encoder *self = (UtUtf8Encoder *)object;

  size_t code_points_length = ut_list_get_length(data);
  for (size_t i = 0; i < code_points_length; i++) {
    uint32_t code_point = ut_uint32_list_get_element(data, i);
    if (code_point <= 0x7f) {
      ut_uint8_list_append(self->buffer, code_point);
    } else if (code_point <= 0x7ff) {
      ut_uint8_list_append(self->buffer, 0xc0 | (code_point >> 6));
      ut_uint8_list_append(self->buffer, 0x80 | (code_point & 0x3f));
    } else if (code_point <= 0xffff) {
      ut_uint8_list_append(self->buffer, 0xe0 | (code_point >> 12));
      ut_uint8_list_append(self->buffer, 0x80 | ((code_point >> 6) & 0x3f));
      ut_uint8_list_append(self->buffer, 0x80 | (code_point & 0x3f));
    } else if (code_point <= 0x10ffff) {
      ut_uint8_list_append(self->buffer, 0xf0 | (code_point >> 18));
      ut_uint8_list_append(self->buffer, 0x80 | ((code_point >> 12) & 0x3f));
      ut_uint8_list_append(self->buffer, 0x80 | ((code_point >> 6) & 0x3f));
      ut_uint8_list_append(self->buffer, 0x80 | (code_point & 0x3f));
    } else {
      assert(false);
    }
  }

  size_t n_used =
      self->callback_object != NULL
          ? self->callback(self->callback_object, self->buffer, complete)
          : 0;
  ut_list_remove(self->buffer, 0, n_used);

  return code_points_length;
}

static void ut_utf8_encoder_read(UtObject *object, UtObject *callback_object,
                                 UtInputStreamCallback callback) {
  UtUtf8Encoder *self = (UtUtf8Encoder *)object;
  assert(callback != NULL);
  assert(self->callback == NULL);
  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callback = callback;
  ut_input_stream_read(self->input, object, read_cb);
}

static void ut_utf8_encoder_close(UtObject *object) {
  UtUtf8Encoder *self = (UtUtf8Encoder *)object;
  ut_input_stream_close(self->input);
}

static UtInputStreamInterface input_stream_interface = {
    .read = ut_utf8_encoder_read, .close = ut_utf8_encoder_close};

static UtObjectInterface object_interface = {
    .type_name = "UtUtf8Encoder",
    .init = ut_utf8_encoder_init,
    .cleanup = ut_utf8_encoder_cleanup,
    .interfaces = {{&ut_input_stream_id, &input_stream_interface},
                   {NULL, NULL}}};

UtObject *ut_utf8_encoder_new(UtObject *input) {
  assert(ut_object_implements_input_stream(input));
  UtObject *object = ut_object_new(sizeof(UtUtf8Encoder), &object_interface);
  UtUtf8Encoder *self = (UtUtf8Encoder *)object;
  self->input = ut_object_ref(input);
  return object;
}

bool ut_object_is_utf8_encoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
