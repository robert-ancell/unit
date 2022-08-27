#include <assert.h>

#include "ut-input-stream.h"
#include "ut-list.h"
#include "ut-uint32-array.h"
#include "ut-uint32-list.h"
#include "ut-uint8-list.h"
#include "ut-utf8-decoder.h"

typedef struct {
  UtObject object;
  UtObject *input;
  UtObject *buffer;

  UtInputStreamDataCallback callback;
  UtInputStreamClosedCallback closed_callback;
  void *user_data;
} UtUtf8Decoder;

static void ut_utf8_decoder_init(UtObject *object) {
  UtUtf8Decoder *self = (UtUtf8Decoder *)object;
  self->buffer = ut_uint32_array_new();
}

static void ut_utf8_decoder_cleanup(UtObject *object) {
  UtUtf8Decoder *self = (UtUtf8Decoder *)object;
  ut_object_unref(self->input);
  ut_object_unref(self->buffer);
}

static size_t read_cb(void *user_data, UtObject *data, bool complete) {
  UtUtf8Decoder *self = user_data;

  size_t utf8_data_length = ut_list_get_length(data);
  size_t offset = 0;
  while (offset < utf8_data_length) {
    uint32_t code_point;
    uint8_t byte1 = ut_uint8_list_get_element(data, offset);
    if ((byte1 & 0x80) == 0) {
      code_point = byte1;
      offset++;
    } else if ((byte1 & 0xe0) == 0xc0) {
      if (offset + 2 > utf8_data_length) {
        break;
      }
      uint8_t byte2 = ut_uint8_list_get_element(data, offset + 1);
      if ((byte2 & 0xc0) != 0x80) {
        code_point = 0xfffd;
      } else {
        code_point = (byte1 & 0x1f) << 6 | (byte2 & 0x3f);
      }
      offset += 2;
    } else if ((byte1 & 0xf0) == 0xe0) {
      if (offset + 3 > utf8_data_length) {
        break;
      }
      uint8_t byte2 = ut_uint8_list_get_element(data, offset + 1);
      uint8_t byte3 = ut_uint8_list_get_element(data, offset + 2);
      if ((byte2 & 0xc0) != 0x80 || (byte3 & 0xc0) != 0x80) {
        code_point = 0xfffd;
      } else {
        code_point =
            (byte1 & 0x0f) << 12 | (byte2 & 0x3f) << 6 | (byte3 & 0x3f);
      }
      offset += 3;
    } else if ((byte1 & 0xf8) == 0xf0) {
      if (offset + 4 > utf8_data_length) {
        break;
      }
      uint8_t byte2 = ut_uint8_list_get_element(data, offset + 1);
      uint8_t byte3 = ut_uint8_list_get_element(data, offset + 2);
      uint8_t byte4 = ut_uint8_list_get_element(data, offset + 3);
      if ((byte2 & 0xc0) != 0x80 || (byte3 & 0xc0) != 0x80 ||
          (byte4 & 0xc0) != 0x80) {
        code_point = 0xfffd;
      } else {
        code_point = (byte1 & 0x07) << 18 | (byte2 & 0x3f) << 12 |
                     (byte3 & 0x3f) << 6 | (byte4 & 0x3f);
      }
      offset += 4;
    } else {
      code_point = 0xfffd;
      offset++;
    }

    ut_uint32_list_append(self->buffer, code_point);
  }

  size_t n_used = self->callback(self->user_data, self->buffer, false);
  ut_list_remove(self->buffer, 0, n_used);

  return offset;
}

static size_t closed_cb(void *user_data, UtObject *data) {
  UtUtf8Decoder *self = user_data;
  self->closed_callback(self->user_data, self->buffer);
  return 0;
}

static void ut_utf8_decoder_read(UtObject *object,
                                 UtInputStreamDataCallback callback,
                                 UtInputStreamClosedCallback closed_callback,
                                 void *user_data, UtObject *cancel) {
  UtUtf8Decoder *self = (UtUtf8Decoder *)object;
  assert(callback != NULL);
  assert(self->callback == NULL);
  self->callback = callback;
  self->closed_callback = closed_callback;
  self->user_data = user_data;
  ut_input_stream_read(self->input, read_cb, closed_cb, self, cancel);
}

static UtInputStreamInterface input_stream_interface = {
    .read = ut_utf8_decoder_read};

static UtObjectInterface object_interface = {
    .type_name = "UtUtf8Decoder",
    .init = ut_utf8_decoder_init,
    .cleanup = ut_utf8_decoder_cleanup,
    .interfaces = {{&ut_input_stream_id, &input_stream_interface},
                   {NULL, NULL}}};

UtObject *ut_utf8_decoder_new(UtObject *input) {
  assert(ut_object_implements_input_stream(input));
  UtObject *object = ut_object_new(sizeof(UtUtf8Decoder), &object_interface);
  UtUtf8Decoder *self = (UtUtf8Decoder *)object;
  self->input = ut_object_ref(input);
  return object;
}

bool ut_object_is_utf8_decoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
