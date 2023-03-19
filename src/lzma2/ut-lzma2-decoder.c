#include <assert.h>

#include "ut.h"

typedef enum {
  DECODER_STATE_BLOCK_HEADER,
  DECODER_STATE_DONE,
  DECODER_STATE_ERROR
} DecoderState;

typedef struct {
  UtObject object;

  // Input stream being read.
  UtObject *input_stream;
  UtObject *read_cancel;

  // Callback for decoded data.
  UtInputStreamCallback callback;
  void *user_data;
  UtObject *cancel;

  // Decoder state
  DecoderState state;

  // Decoded data.
  UtObject *buffer;
  size_t buffer_read_offset;

  // Error that occurred during decoding.
  UtObject *error;
} UtLzma2Decoder;

static void set_error(UtLzma2Decoder *self, const char *description) {
  if (self->state == DECODER_STATE_ERROR) {
    return;
  }

  self->error = ut_lzma2_error_new(description);
  self->state = DECODER_STATE_ERROR;
}

static size_t read_cb(void *user_data, UtObject *data, bool complete) {
  UtLzma2Decoder *self = user_data;

  if (ut_cancel_is_active(self->cancel)) {
    ut_cancel_activate(self->read_cancel);
    return 0;
  }

  size_t offset = 0;
  bool decoding = true;
  while (decoding) {
    switch (self->state) {
    case DECODER_STATE_DONE:
      ut_cancel_activate(self->read_cancel);
      decoding = false;
      break;
    case DECODER_STATE_ERROR:
      ut_cancel_activate(self->read_cancel);
      self->callback(self->user_data, self->error, true);
      return offset;
    }
  }

  size_t buffer_length = ut_list_get_length(self->buffer);
  UtObjectRef unread_buffer =
      ut_list_get_sublist(self->buffer, self->buffer_read_offset,
                          buffer_length - self->buffer_read_offset);
  size_t n_used = self->callback(self->user_data, unread_buffer,
                                 self->state == DECODER_STATE_DONE);
  self->buffer_read_offset += n_used;

  return offset;
}

static void ut_lzma2_decoder_init(UtObject *object) {
  UtLzma2Decoder *self = (UtLzma2Decoder *)object;
  self->read_cancel = ut_cancel_new();
  self->state = DECODER_STATE_BLOCK_HEADER;
  self->buffer = ut_uint8_array_new();
}

static void ut_lzma2_decoder_cleanup(UtObject *object) {
  UtLzma2Decoder *self = (UtLzma2Decoder *)object;
  ut_cancel_activate(self->read_cancel);
  ut_object_unref(self->input_stream);
  ut_object_unref(self->read_cancel);
  ut_object_unref(self->cancel);
  ut_object_unref(self->buffer);
  ut_object_unref(self->error);
}

static void ut_lzma2_decoder_read(UtObject *object,
                                  UtInputStreamCallback callback,
                                  void *user_data, UtObject *cancel) {
  UtLzma2Decoder *self = (UtLzma2Decoder *)object;
  assert(callback != NULL);
  assert(self->callback == NULL);
  self->callback = callback;
  self->user_data = user_data;
  self->cancel = ut_object_ref(cancel);
  ut_input_stream_read(self->input_stream, read_cb, self, self->read_cancel);
}

static UtInputStreamInterface input_stream_interface = {
    .read = ut_lzma2_decoder_read};

static UtObjectInterface object_interface = {
    .type_name = "UtLzma2Decoder",
    .init = ut_lzma2_decoder_init,
    .cleanup = ut_lzma2_decoder_cleanup,
    .interfaces = {{&ut_input_stream_id, &input_stream_interface},
                   {NULL, NULL}}};

UtObject *ut_lzma2_decoder_new(UtObject *input_stream) {
  assert(input_stream != NULL);
  UtObject *object = ut_object_new(sizeof(UtLzma2Decoder), &object_interface);
  UtLzma2Decoder *self = (UtLzma2Decoder *)object;
  self->input_stream = ut_object_ref(input_stream);
  return object;
}

bool ut_object_is_lzma2_decoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
