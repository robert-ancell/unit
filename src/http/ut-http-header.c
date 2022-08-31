#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "ut.h"

typedef struct {
  UtObject object;
  char *name;
  char *value;
} UtHttpHeader;

static void ut_http_header_cleanup(UtObject *object) {
  UtHttpHeader *self = (UtHttpHeader *)object;
  free(self->name);
  free(self->value);
}

static UtObjectInterface object_interface = {.type_name = "HttpHeader",
                                             .cleanup = ut_http_header_cleanup};

UtObject *ut_http_header_new(const char *name, const char *value) {
  UtObject *object = ut_object_new(sizeof(UtHttpHeader), &object_interface);
  UtHttpHeader *self = (UtHttpHeader *)object;
  self->name = ut_cstring_new(name);
  self->value = ut_cstring_new(value);
  return object;
}

const char *ut_http_header_get_name(UtObject *object) {
  assert(ut_object_is_http_header(object));
  UtHttpHeader *self = (UtHttpHeader *)object;
  return self->name;
}

const char *ut_http_header_get_value(UtObject *object) {
  assert(ut_object_is_http_header(object));
  UtHttpHeader *self = (UtHttpHeader *)object;
  return self->value;
}

bool ut_object_is_http_header(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
