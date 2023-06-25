#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;

  char *method;
  char *path;
  UtObject *headers;
  UtObject *body;
} UtHttpRequest;

static char *ut_http_request_to_string(UtObject *object) {
  UtHttpRequest *self = (UtHttpRequest *)object;
  ut_cstring_ref headers_string = ut_object_to_string(self->headers);
  return ut_cstring_new_printf("<UtHttpRequest>(\"%s\", \"%s\", %s)",
                               self->method, self->path, headers_string);
}

static void ut_http_request_cleanup(UtObject *object) {
  UtHttpRequest *self = (UtHttpRequest *)object;
  free(self->method);
  free(self->path);
  ut_object_unref(self->headers);
  ut_object_unref(self->body);
}

static UtObjectInterface object_interface = {
    .type_name = "UtHttpRequest",
    .to_string = ut_http_request_to_string,
    .cleanup = ut_http_request_cleanup};

UtObject *ut_http_request_new(const char *method, const char *path,
                              UtObject *headers, UtObject *body) {
  UtObject *object = ut_object_new(sizeof(UtHttpRequest), &object_interface);
  UtHttpRequest *self = (UtHttpRequest *)object;
  self->method = ut_cstring_new(method);
  self->path = ut_cstring_new(path);
  self->headers = ut_object_ref(headers);
  self->body = ut_object_ref(body);
  return object;
}

const char *ut_http_request_get_method(UtObject *object) {
  assert(ut_object_is_http_request(object));
  UtHttpRequest *self = (UtHttpRequest *)object;
  return self->method;
}

const char *ut_http_request_get_path(UtObject *object) {
  assert(ut_object_is_http_request(object));
  UtHttpRequest *self = (UtHttpRequest *)object;
  return self->path;
}

UtObject *ut_http_request_get_headers(UtObject *object) {
  assert(ut_object_is_http_request(object));
  UtHttpRequest *self = (UtHttpRequest *)object;
  return self->headers;
}

UtObject *ut_http_request_get_body(UtObject *object) {
  assert(ut_object_is_http_request(object));
  UtHttpRequest *self = (UtHttpRequest *)object;
  return self->body;
}

bool ut_object_is_http_request(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
