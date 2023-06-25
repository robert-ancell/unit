#include <assert.h>

#include "ut-http-message-decoder.h"
#include "ut-http-message-encoder.h"
#include "ut-http-server-client.h"
#include "ut.h"

typedef struct {
  UtObject object;

  UtObject *input_stream;

  // Callback to notify when requests come in.
  UtObject *callback_object;
  UtHttpServerClientRequestCallback callback;

  UtObject *message_input_stream;
  UtObject *message_decoder;
} UtHttpServerClient;

static size_t http_read_cb(UtObject *object, UtObject *data, bool complete) {
  UtHttpServerClient *self = (UtHttpServerClient *)object;

  size_t n_read = ut_writable_input_stream_write(self->message_input_stream,
                                                 data, complete);
  if (ut_http_message_decoder_get_headers_done(self->message_decoder)) {
    if (self->callback_object != NULL && self->callback != NULL) {
      UtObjectRef request = ut_http_request_new(
          ut_http_message_decoder_get_method(self->message_decoder),
          ut_http_message_decoder_get_path(self->message_decoder),
          ut_http_message_decoder_get_headers(self->message_decoder),
          ut_http_message_decoder_get_body(self->message_decoder));
      self->callback(self->callback_object, request);
    }
  }

  return n_read;
}

static void ut_http_server_client_client_init(UtObject *object) {
  UtHttpServerClient *self = (UtHttpServerClient *)object;
  self->message_input_stream = ut_writable_input_stream_new();
}

static void ut_http_server_client_client_cleanup(UtObject *object) {
  UtHttpServerClient *self = (UtHttpServerClient *)object;
  ut_object_unref(self->input_stream);
  ut_object_weak_unref(&self->callback_object);
  ut_object_unref(self->message_input_stream);
  ut_object_unref(self->message_decoder);
}

static UtObjectInterface object_interface = {
    .type_name = "UtHttpServerClient",
    .init = ut_http_server_client_client_init,
    .cleanup = ut_http_server_client_client_cleanup};

UtObject *
ut_http_server_client_new(UtObject *input_stream, UtObject *callback_object,
                          UtHttpServerClientRequestCallback callback) {
  UtObject *object =
      ut_object_new(sizeof(UtHttpServerClient), &object_interface);
  UtHttpServerClient *self = (UtHttpServerClient *)object;

  self->input_stream = ut_object_ref(input_stream);
  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callback = callback;
  self->message_decoder =
      ut_http_message_decoder_new_request(self->message_input_stream);
  ut_http_message_decoder_read(self->message_decoder);

  return object;
}

void ut_http_server_client_read(UtObject *object) {
  assert(ut_object_is_http_server_client(object));
  UtHttpServerClient *self = (UtHttpServerClient *)object;
  ut_input_stream_read(self->input_stream, object, http_read_cb);
}

void ut_http_server_client_send_response(UtObject *object, UtObject *response) {
  assert(ut_object_is_http_server_client(object));
  UtHttpServerClient *self = (UtHttpServerClient *)object;

  UtObjectRef message_encoder = ut_http_message_encoder_new_response(
      self->input_stream, ut_http_response_get_status_code(response),
      ut_http_response_get_reason_phrase(response),
      ut_http_response_get_headers(response),
      ut_http_response_get_body(response));
  ut_http_message_encoder_encode(message_encoder);
}

bool ut_object_is_http_server_client(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
