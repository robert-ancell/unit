#include <assert.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>

#include "ut.h"

typedef struct {
  UtObject object;
  unsigned int status_code;
  char *reason_phrase;
  UtObject *headers;
  UtObject *body;
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

static void ut_http_response_cleanup(UtObject *object) {
  UtHttpResponse *self = (UtHttpResponse *)object;
  free(self->reason_phrase);
  ut_object_unref(self->headers);
  ut_object_unref(self->body);
}

static UtObjectInterface object_interface = {.type_name = "UtHttpResponse",
                                             .cleanup =
                                                 ut_http_response_cleanup,
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_http_response_new(unsigned int status_code,
                               const char *reason_phrase, UtObject *headers,
                               UtObject *body) {
  UtObject *object = ut_object_new(sizeof(UtHttpResponse), &object_interface);
  UtHttpResponse *self = (UtHttpResponse *)object;
  self->status_code = status_code;
  self->reason_phrase = ut_cstring_new(reason_phrase);
  self->headers = ut_object_ref(headers);
  self->body = ut_object_ref(body);
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

UtObject *ut_http_response_get_body(UtObject *object) {
  assert(ut_object_is_http_response(object));
  UtHttpResponse *self = (UtHttpResponse *)object;
  return self->body;
}

bool ut_object_is_http_response(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
