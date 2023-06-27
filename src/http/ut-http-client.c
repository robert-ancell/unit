#include <assert.h>
#include <stdlib.h>
#include <sys/types.h>

#include "ut-http-message-decoder.h"
#include "ut-http-message-encoder.h"
#include "ut.h"

// FIXME: Make a real object
typedef struct {
  UtObject *tcp_socket;
  UtObject *cancel;
  char *host;
  uint16_t port;
  char *method;
  char *path;
  UtObject *body;
  UtHttpResponseCallback callback;
  void *callback_user_data;
  UtObject *callback_cancel;
  UtObject *message_encoder;
  UtObject *message_decoder_input_stream;
  UtObject *message_decoder;
} HttpRequest;

HttpRequest *http_request_new(const char *host, uint16_t port,
                              const char *method, const char *path,
                              UtObject *body, UtHttpResponseCallback callback,
                              void *callback_user_data,
                              UtObject *callback_cancel) {
  HttpRequest *request = malloc(sizeof(HttpRequest));
  request->host = ut_cstring_new(host);
  request->port = port;
  request->method = ut_cstring_new(method);
  request->path = ut_cstring_new(path);
  request->body = ut_object_ref(body);
  request->cancel = ut_cancel_new();
  request->callback = callback;
  request->callback_user_data = callback_user_data;
  request->callback_cancel = ut_object_ref(callback_cancel);
  request->message_decoder_input_stream = ut_writable_input_stream_new();
  request->message_decoder = ut_http_message_decoder_new_response(
      request->message_decoder_input_stream);
  return request;
}

void http_request_free(HttpRequest *request) {
  ut_object_unref(request->tcp_socket);
  ut_object_unref(request->cancel);
  free(request->host);
  free(request->method);
  free(request->path);
  ut_object_unref(request->body);
  ut_object_unref(request->callback_cancel);
  ut_object_unref(request->message_encoder);
  ut_object_unref(request->message_decoder_input_stream);
  ut_object_unref(request->message_decoder);
  free(request);
}

typedef struct {
  UtObject object;
  UtObject *ip_address_resolver;
} UtHttpClient;

static void ut_http_client_init(UtObject *object) {
  UtHttpClient *self = (UtHttpClient *)object;
  self->ip_address_resolver = ut_ip_address_resolver_new();
}

static void ut_http_client_cleanup(UtObject *object) {
  UtHttpClient *self = (UtHttpClient *)object;
  ut_object_unref(self->ip_address_resolver);
}

static size_t read_cb(void *user_data, UtObject *data, bool complete) {
  HttpRequest *request = user_data;

  bool headers_done =
      ut_http_message_decoder_get_headers_done(request->message_decoder);
  size_t n_used = ut_writable_input_stream_write(
      request->message_decoder_input_stream, data, complete);
  if (!headers_done &&
      ut_http_message_decoder_get_headers_done(request->message_decoder)) {
    UtObjectRef response = ut_http_response_new(
        ut_http_message_decoder_get_status_code(request->message_decoder),
        ut_http_message_decoder_get_reason_phrase(request->message_decoder),
        ut_http_message_decoder_get_headers(request->message_decoder),
        ut_http_message_decoder_get_body(request->message_decoder));
    if (request->callback != NULL) {
      request->callback(request->callback_user_data, response);
    }
  }

  return n_used;
}

static void connect_cb(void *user_data) {
  HttpRequest *request = user_data;

  UtObjectRef headers = ut_list_new();
  ut_list_append_take(headers, ut_http_header_new("Host", request->host));
  request->message_encoder = ut_http_message_encoder_new_request(
      request->tcp_socket, request->method, request->path, headers,
      request->body);
  ut_http_message_encoder_encode(request->message_encoder);
  ut_http_message_decoder_read(request->message_decoder);
  ut_input_stream_read(request->tcp_socket, read_cb, request, request->cancel);
}

static void lookup_cb(void *user_data, UtObject *addresses) {
  HttpRequest *request = user_data;

  UtObjectRef address = ut_list_get_first(addresses);
  request->tcp_socket = ut_tcp_socket_new(address, request->port);
  ut_tcp_socket_connect(request->tcp_socket, connect_cb, request,
                        request->callback_cancel);
}

static UtObjectInterface object_interface = {.type_name = "UtHttpClient",
                                             .init = ut_http_client_init,
                                             .cleanup = ut_http_client_cleanup};

UtObject *ut_http_client_new() {
  return ut_object_new(sizeof(UtHttpClient), &object_interface);
}

void ut_http_client_send_request(UtObject *object, const char *method,
                                 const char *uri, UtObject *body,
                                 UtHttpResponseCallback callback,
                                 void *user_data, UtObject *cancel) {
  assert(ut_object_is_http_client(object));
  UtHttpClient *self = (UtHttpClient *)object;

  UtObjectRef uri_object = ut_uri_new_from_string(uri);
  assert(!ut_object_implements_error(uri_object));
  assert(ut_cstring_equal(ut_uri_get_scheme(uri_object), "http"));
  const char *host = ut_uri_get_host(uri_object);
  assert(host != NULL);
  const char *path = ut_uri_get_path(uri_object);
  if (path == NULL || ut_cstring_equal(path, "")) {
    path = "/";
  }
  uint16_t port = ut_uri_get_port(uri_object);
  if (port == 0) {
    port = 80;
  }

  HttpRequest *request = http_request_new(host, port, method, path, body,
                                          callback, user_data, cancel);
  ut_ip_address_resolver_lookup(self->ip_address_resolver, host, lookup_cb,
                                request, cancel);
}

bool ut_object_is_http_client(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
