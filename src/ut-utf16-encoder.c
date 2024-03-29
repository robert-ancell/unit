#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *input;
  UtObject *buffer;

  UtObject *callback_object;
  UtInputStreamCallback callback;
} UtUtf16Encoder;

static void ut_utf16_encoder_init(UtObject *object) {
  UtUtf16Encoder *self = (UtUtf16Encoder *)object;
  self->buffer = ut_uint16_list_new();
}

static void ut_utf16_encoder_cleanup(UtObject *object) {
  UtUtf16Encoder *self = (UtUtf16Encoder *)object;
  ut_object_unref(self->input);
  ut_object_unref(self->buffer);
  ut_object_weak_unref(&self->callback_object);
}

static size_t read_cb(UtObject *object, UtObject *data, bool complete) {
  UtUtf16Encoder *self = (UtUtf16Encoder *)object;

  size_t code_points_length = ut_list_get_length(data);
  for (size_t i = 0; i < code_points_length; i++) {
    uint32_t code_point = ut_uint32_list_get_element(data, i);
    if (code_point <= 0xd7ff ||
        (code_point >= 0xe000 && code_point <= 0xffff)) {
      ut_uint16_list_append(self->buffer, code_point);
    } else if (code_point >= 0x10000 && code_point <= 0x10ffff) {
      uint32_t c = code_point - 0x10000;
      ut_uint16_list_append(self->buffer, 0xd800 | (c >> 10));
      ut_uint16_list_append(self->buffer, 0xdc00 | (c & 0x3ff));
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

static void ut_utf16_encoder_read(UtObject *object, UtObject *callback_object,
                                  UtInputStreamCallback callback) {
  UtUtf16Encoder *self = (UtUtf16Encoder *)object;
  assert(callback != NULL);
  assert(self->callback == NULL);
  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callback = callback;
  ut_input_stream_read(self->input, object, read_cb);
}

static void ut_utf16_encoder_close(UtObject *object) {
  UtUtf16Encoder *self = (UtUtf16Encoder *)object;
  ut_input_stream_close(self->input);
}

static UtInputStreamInterface input_stream_interface = {
    .read = ut_utf16_encoder_read, .close = ut_utf16_encoder_close};

static UtObjectInterface object_interface = {
    .type_name = "UtUtf16Encoder",
    .init = ut_utf16_encoder_init,
    .cleanup = ut_utf16_encoder_cleanup,
    .interfaces = {{&ut_input_stream_id, &input_stream_interface},
                   {NULL, NULL}}};

UtObject *ut_utf16_encoder_new(UtObject *input) {
  assert(ut_object_implements_input_stream(input));
  UtObject *object = ut_object_new(sizeof(UtUtf16Encoder), &object_interface);
  UtUtf16Encoder *self = (UtUtf16Encoder *)object;
  self->input = ut_object_ref(input);
  return object;
}

bool ut_object_is_utf16_encoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
