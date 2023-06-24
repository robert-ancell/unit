#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;

  UtObject *cancel;
  char *host;
  uint16_t port;
  char *method;
  char *path;
  UtObject *headers;
  UtObject *body;
  UtHttpResponseCallback callback;
  void *callback_user_data;
  UtObject *callback_cancel;
  UtObject *message_encoder;
  UtObject *message_decoder_input_stream;
  UtObject *message_decoder;
} UtHttpRequest;

static void ut_http_request_cleanup(UtObject *object) {
  UtHttpRequest *self = (UtHttpRequest *)object;
  ut_object_unref(self->cancel);
  free(self->host);
  free(self->method);
  free(self->path);
  ut_object_unref(self->body);
  ut_object_unref(self->callback_cancel);
  ut_object_unref(self->message_encoder);
  ut_object_unref(self->message_decoder_input_stream);
  ut_object_unref(self->message_decoder);
}

static UtObjectInterface object_interface = {
    .type_name = "UtHttpRequest", .cleanup = ut_http_request_cleanup};

UtObject *ut_http_request_new(const char *host, uint16_t port,
                              const char *method, const char *path,
                              UtObject *headers, UtObject *body,
                              UtHttpResponseCallback callback,
                              void *callback_user_data,
                              UtObject *callback_cancel) {
  UtObject *object = ut_object_new(sizeof(UtHttpRequest), &object_interface);
  UtHttpRequest *self = (UtHttpRequest *)object;
  self->host = ut_cstring_new(host);
  self->port = port;
  self->method = ut_cstring_new(method);
  self->path = ut_cstring_new(path);
  self->headers = ut_object_ref(headers);
  self->body = ut_object_ref(body);
  self->cancel = ut_cancel_new();
  self->callback = callback;
  self->callback_user_data = callback_user_data;
  self->callback_cancel = ut_object_ref(callback_cancel);
  return object;
}

const char *ut_http_request_get_host(UtObject *object) {
  assert(ut_object_is_http_request(object));
  UtHttpRequest *self = (UtHttpRequest *)object;
  return self->host;
}

uint16_t ut_http_request_get_port(UtObject *object) {
  assert(ut_object_is_http_request(object));
  UtHttpRequest *self = (UtHttpRequest *)object;
  return self->port;
}

bool ut_object_is_http_request(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
