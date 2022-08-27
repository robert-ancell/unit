#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>

#include "ut-cancel.h"
#include "ut-error.h"
#include "ut-general-error.h"
#include "ut-http-header.h"
#include "ut-http-response.h"
#include "ut-input-stream.h"
#include "ut-list.h"
#include "ut-object-list.h"
#include "ut-tcp-client.h"

typedef struct {
  UtObject object;
  unsigned int status_code;
  char *reason_phrase;
  UtObject *headers;
  UtObject *tcp_client;
  size_t n_read;
  UtObject *read_cancel;
  UtInputStreamDataCallback callback;
  UtInputStreamClosedCallback closed_callback;
  void *user_data;
  UtObject *cancel;
} UtHttpResponse;

static const char *get_header(UtHttpResponse *self, const char *name) {
  assert(name != NULL);
  for (size_t i = 0; i < ut_list_get_length(self->headers); i++) {
    UtObject *header = ut_object_list_get_element(self->headers, i);
    if (strcasecmp(ut_http_header_get_name(header), name) == 0) {
      return ut_http_header_get_value(header);
    }
  }

  return NULL;
}

static ssize_t get_content_length(UtHttpResponse *self) {
  const char *value = get_header(self, "Content-Length");
  if (value == NULL) {
    return -1;
  }

  return atoi(value);
}

static size_t read_cb(void *user_data, UtObject *data, bool complete) {
  UtHttpResponse *self = user_data;

  // Stop listening for input when done.
  if (ut_cancel_is_active(self->cancel)) {
    ut_cancel_activate(self->read_cancel);
    return 0;
  }

  if (ut_object_implements_error(data)) {
    self->callback(self->user_data, data, true);
    return 0;
  }

  size_t n_used = 0;
  size_t data_length = ut_list_get_length(data);
  ssize_t content_length = get_content_length(self);
  if (content_length >= 0) {
    bool have_all_data = self->n_read + data_length >= (size_t)content_length;

    // Read no more data.
    ut_cancel_activate(self->read_cancel);

    UtObjectRef last_block =
        ut_list_get_sublist(data, 0, content_length - self->n_read);
    n_used = self->callback(self->user_data, last_block, have_all_data);

    // All data consumed!
    if (self->n_read + n_used >= content_length) {
      ut_cancel_activate(self->read_cancel);
    }

    // Generate error if didn't receive enough data.
    if (!have_all_data && complete && !ut_cancel_is_active(self->cancel)) {
      UtObjectRef error =
          ut_general_error_new("Connection closed before end of content");
      self->callback(self->user_data, error, true);
      ut_cancel_activate(self->read_cancel);
    }
  } else {
    n_used = self->callback(self->user_data, data, complete);
  }

  self->n_read += n_used;

  // Stop listening for input when done.
  if (ut_cancel_is_active(self->cancel)) {
    ut_cancel_activate(self->read_cancel);
  }

  return n_used;
}

static size_t closed_cb(void *user_data, UtObject *data) {
  UtHttpResponse *self = user_data;
  return self->closed_callback(self->user_data, data);
}

static void start_read(UtHttpResponse *self, UtInputStreamDataCallback callback,
                       UtInputStreamClosedCallback closed_callback,
                       void *user_data, UtObject *cancel) {
  // Clean up after the previous read.
  if (ut_cancel_is_active(self->cancel)) {
    ut_object_clear(&self->read_cancel);
    self->callback = NULL;
    self->user_data = NULL;
    ut_object_clear(&self->cancel);
  }

  assert(self->callback == NULL);

  self->read_cancel = ut_cancel_new();
  self->callback = callback;
  self->closed_callback = closed_callback;
  self->user_data = user_data;
  self->cancel = ut_object_ref(cancel);

  ut_input_stream_read(self->tcp_client, read_cb, closed_cb, self,
                       self->read_cancel);
}

static void ut_http_response_cleanup(UtObject *object) {
  UtHttpResponse *self = (UtHttpResponse *)object;
  free(self->reason_phrase);
  ut_object_unref(self->headers);
  ut_object_unref(self->tcp_client);
  ut_object_unref(self->cancel);
}

static void ut_http_response_read(UtObject *object,
                                  UtInputStreamDataCallback callback,
                                  UtInputStreamClosedCallback closed_callback,
                                  void *user_data, UtObject *cancel) {
  UtHttpResponse *self = (UtHttpResponse *)object;
  assert(callback != NULL);

  start_read(self, callback, closed_callback, user_data, cancel);
}

static UtInputStreamInterface input_stream_interface = {
    .read = ut_http_response_read};

static UtObjectInterface object_interface = {
    .type_name = "HttpResponse",
    .cleanup = ut_http_response_cleanup,
    .interfaces = {{&ut_input_stream_id, &input_stream_interface},
                   {NULL, NULL}}};

UtObject *ut_http_response_new(unsigned int status_code,
                               const char *reason_phrase, UtObject *headers,
                               UtObject *tcp_client) {
  UtObject *object = ut_object_new(sizeof(UtHttpResponse), &object_interface);
  UtHttpResponse *self = (UtHttpResponse *)object;
  self->status_code = status_code;
  self->reason_phrase = strdup(reason_phrase);
  self->headers = ut_object_ref(headers);
  self->tcp_client = ut_object_ref(tcp_client);
  return object;
}

unsigned int ut_http_response_get_status_code(UtObject *object) {
  assert(ut_object_is_http_response(object));
  UtHttpResponse *self = (UtHttpResponse *)object;
  return self->status_code;
}

const char *ut_http_response_get_reason_phrase(UtObject *object) {
  assert(ut_object_is_http_response(object));
  UtHttpResponse *self = (UtHttpResponse *)object;
  return self->reason_phrase;
}

UtObject *ut_http_response_get_headers(UtObject *object) {
  assert(ut_object_is_http_response(object));
  UtHttpResponse *self = (UtHttpResponse *)object;
  return self->headers;
}

const char *ut_http_response_get_header(UtObject *object, const char *name) {
  assert(ut_object_is_http_response(object));
  UtHttpResponse *self = (UtHttpResponse *)object;
  return get_header(self, name);
}

ssize_t ut_http_response_get_content_length(UtObject *object) {
  assert(ut_object_is_http_response(object));
  UtHttpResponse *self = (UtHttpResponse *)object;
  return get_content_length(self);
}

bool ut_object_is_http_response(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
