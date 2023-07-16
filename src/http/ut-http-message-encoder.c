#include <assert.h>

#include "ut-http-message-encoder.h"
#include "ut.h"

typedef enum {
  BODY_LENGTH_FORMAT_EOF,
  BODY_LENGTH_FORMAT_FIXED,
  BODY_LENGTH_FORMAT_CHUNKED
} BodyLengthFormat;

typedef struct {
  UtObject object;

  // Stream write HTTP message to.
  UtObject *output_stream;

  // Reported method.
  char *method;

  // Reported path.
  char *path;

  // Reported status code.
  unsigned int status_code;

  // Reported reason phrase.
  char *reason_phrase;

  // Message headers.
  UtObject *headers;

  // Message body;
  UtObject *body;

  // Method of writing body.
  BodyLengthFormat body_length_format;

  // Length of body when [length_format] is FIXED.
  size_t content_length;

  // Number of bytes written from [body].
  size_t body_length;
} UtHttpMessageEncoder;

static UtObject *find_header(UtHttpMessageEncoder *self, const char *name) {
  size_t headers_length = ut_list_get_length(self->headers);
  for (size_t i = 0; i < headers_length; i++) {
    UtObject *header = ut_object_list_get_element(self->headers, i);
    // FIXME: Case insensiive
    if (ut_cstring_equal(ut_http_header_get_name(header), name)) {
      return header;
    }
  }

  return NULL;
}

static size_t write_body_eof(UtHttpMessageEncoder *self, UtObject *data) {
  ut_output_stream_write(self->output_stream, data);
  return ut_list_get_length(data);
}

static size_t write_body_fixed(UtHttpMessageEncoder *self, UtObject *data) {
  size_t data_length = ut_list_get_length(data);
  size_t n_remaining = self->content_length - self->body_length;
  if (n_remaining == 0) {
    return 0;
  } else if (data_length <= n_remaining) {
    ut_output_stream_write(self->output_stream, data);
    self->body_length += data_length;
    return data_length;
  } else {
    UtObjectRef d = ut_list_get_sublist(data, 0, n_remaining);
    ut_output_stream_write(self->output_stream, d);
    self->body_length += n_remaining;
    return n_remaining;
  }
}

static size_t write_body_chunked(UtHttpMessageEncoder *self, UtObject *data,
                                 bool complete) {
  size_t data_length = ut_list_get_length(data);

  if (data_length > 0) {
    UtObjectRef chunk_start = ut_string_new_printf("%lx\r\n", data_length);
    UtObjectRef chunk_start_data = ut_string_get_utf8(chunk_start);
    ut_output_stream_write(self->output_stream, chunk_start_data);

    ut_output_stream_write(self->output_stream, data);

    UtObjectRef chunk_end = ut_string_new("\r\n");
    UtObjectRef chunk_end_data = ut_string_get_utf8(chunk_end);
    ut_output_stream_write(self->output_stream, chunk_end_data);
  }

  if (complete) {
    UtObjectRef terminating_chunk = ut_string_new("0\r\n\r\n");
    UtObjectRef terminating_chunk_data = ut_string_get_utf8(terminating_chunk);
    ut_output_stream_write(self->output_stream, terminating_chunk_data);
  }

  return data_length;
}

static size_t body_read_cb(void *user_data, UtObject *data, bool complete) {
  UtHttpMessageEncoder *self = user_data;

  switch (self->body_length_format) {
  case BODY_LENGTH_FORMAT_EOF:
    return write_body_eof(self, data);
  case BODY_LENGTH_FORMAT_FIXED:
    return write_body_fixed(self, data);
    break;
  case BODY_LENGTH_FORMAT_CHUNKED:
    return write_body_chunked(self, data, complete);
    break;
  default:
    return 0;
  }
}

static void ut_http_message_encoder_cleanup(UtObject *object) {
  UtHttpMessageEncoder *self = (UtHttpMessageEncoder *)object;

  ut_input_stream_close(self->body);

  ut_object_unref(self->output_stream);
  free(self->method);
  free(self->path);
  free(self->reason_phrase);
  ut_object_unref(self->headers);
  ut_object_unref(self->body);
}

static UtObjectInterface object_interface = {
    .type_name = "UtHttpMessageEncoder",
    .cleanup = ut_http_message_encoder_cleanup};

UtObject *ut_http_message_encoder_new_request(UtObject *output_stream,
                                              const char *method,
                                              const char *path,
                                              UtObject *headers,
                                              UtObject *body) {
  UtObject *object =
      ut_object_new(sizeof(UtHttpMessageEncoder), &object_interface);
  UtHttpMessageEncoder *self = (UtHttpMessageEncoder *)object;
  self->output_stream = ut_object_ref(output_stream);
  self->method = ut_cstring_new(method);
  self->path = ut_cstring_new(path);
  self->headers = ut_object_ref(headers);
  self->body = ut_object_ref(body);
  return object;
}

UtObject *ut_http_message_encoder_new_response(UtObject *output_stream,
                                               unsigned int status_code,
                                               const char *reason_phrase,
                                               UtObject *headers,
                                               UtObject *body) {
  UtObject *object =
      ut_object_new(sizeof(UtHttpMessageEncoder), &object_interface);
  UtHttpMessageEncoder *self = (UtHttpMessageEncoder *)object;
  self->output_stream = ut_object_ref(output_stream);
  self->status_code = status_code;
  self->reason_phrase = ut_cstring_new(reason_phrase);
  self->headers = ut_object_ref(headers);
  self->body = ut_object_ref(body);
  return object;
}

void ut_http_message_encoder_encode(UtObject *object) {
  assert(ut_object_is_http_message_encoder(object));
  UtHttpMessageEncoder *self = (UtHttpMessageEncoder *)object;

  UtObjectRef header = ut_string_new("");
  if (self->method != NULL) {
    ut_string_append(header, self->method);
    ut_string_append(header, " ");
    ut_string_append(header, self->path);
    ut_string_append(header, " HTTP/1.1\r\n");
  } else {
    ut_string_append(header, "HTTP/1.1 ");
    ut_string_append_printf(header, "%d", self->status_code);
    ut_string_append(header, " ");
    ut_string_append(header, self->reason_phrase);
    ut_string_append(header, "\r\n");
  }

  size_t header_length = ut_list_get_length(self->headers);
  for (size_t i = 0; i < header_length; i++) {
    UtObjectRef h = ut_list_get_element(self->headers, i);
    ut_string_append(header, ut_http_header_get_name(h));
    ut_string_append(header, ": ");
    ut_string_append(header, ut_http_header_get_value(h));
    ut_string_append(header, "\r\n");
  }
  ut_string_append(header, "\r\n");

  UtObjectRef header_data = ut_string_get_utf8(header);
  ut_output_stream_write(self->output_stream, header_data);

  UtObject *content_length_header = find_header(self, "Content-Length");
  // FIXME: Handle more complex transfer encodings
  // FIXME: Validate not both content-length and chunked transfer encoding.
  UtObject *transfer_encoding_header = find_header(self, "Transfer-Encoding");
  if (content_length_header != NULL) {
    self->body_length_format = BODY_LENGTH_FORMAT_FIXED;
    self->content_length =
        strtoul(ut_http_header_get_value(content_length_header), NULL, 10);
  } else if (transfer_encoding_header != NULL &&
             ut_cstring_equal(
                 ut_http_header_get_value(transfer_encoding_header),
                 "chunked")) {
    self->body_length_format = BODY_LENGTH_FORMAT_CHUNKED;
  } else {
    self->body_length_format = BODY_LENGTH_FORMAT_EOF;
  }

  if (self->body != NULL) {
    ut_input_stream_read(self->body, body_read_cb, self);
  } else {
    UtObjectRef d = ut_uint8_list_new();
    body_read_cb(self, d, true);
  }
}

bool ut_object_is_http_message_encoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
