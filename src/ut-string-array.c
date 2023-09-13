#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>

#include "ut-list-private.h"
#include "ut-string-subarray.h"
#include "ut.h"

typedef struct {
  UtObject object;
  char **data;
  size_t data_length;
} UtStringArray;

static void resize(UtStringArray *self, size_t length) {
  for (size_t i = length; i < self->data_length; i++) {
    free(self->data[i]);
  }
  self->data = realloc(self->data, sizeof(char *) * (length + 1));
  for (size_t i = self->data_length; i < length; i++) {
    self->data[i] = ut_cstring_new("");
  }
  self->data[length] = NULL;
  self->data_length = length;
}

static void insert(UtStringArray *self, size_t index, const char *value) {
  self->data_length++;
  self->data = realloc(self->data, sizeof(char *) * (self->data_length + 1));
  for (size_t i = self->data_length - 1; i > index; i--) {
    self->data[i] = self->data[i - 1];
  }
  self->data[index] = ut_cstring_new(value);
  self->data[self->data_length] = NULL;
}

static size_t ut_string_array_get_length(UtObject *object) {
  UtStringArray *self = (UtStringArray *)object;
  return self->data_length;
}

static UtObject *ut_string_array_get_element_object(UtObject *object,
                                                    size_t index) {
  UtStringArray *self = (UtStringArray *)object;
  return ut_string_new(self->data[index]);
}

static UtObject *ut_string_array_get_sublist(UtObject *object, size_t start,
                                             size_t count) {
  return ut_string_subarray_new(object, start, count);
}

static UtObject *ut_string_array_copy(UtObject *object) {
  UtStringArray *self = (UtStringArray *)object;
  UtStringArray *copy = (UtStringArray *)ut_string_array_new();
  copy->data = malloc(sizeof(char *) * (self->data_length + 1));
  copy->data_length = self->data_length;
  for (size_t i = 0; i < self->data_length; i++) {
    copy->data[i] = ut_cstring_new(self->data[i]);
  }
  copy->data[copy->data_length] = NULL;
  return (UtObject *)copy;
}

static void ut_string_array_insert_object(UtObject *object, size_t index,
                                          UtObject *item) {
  UtStringArray *self = (UtStringArray *)object;
  assert(ut_object_implements_string(item));
  insert(self, index, ut_string_get_text(item));
}

static void ut_string_array_remove(UtObject *object, size_t index,
                                   size_t count) {
  UtStringArray *self = (UtStringArray *)object;
  assert(index <= self->data_length);
  assert(index + count <= self->data_length);
  for (size_t i = index; i <= self->data_length - count; i++) {
    self->data[i] = self->data[i + count];
  }
  resize(self, self->data_length - count);
}

static void ut_string_array_resize(UtObject *object, size_t length) {
  UtStringArray *self = (UtStringArray *)object;
  resize(self, length);
}

static const char *ut_string_array_get_element(UtObject *object, size_t index) {
  UtStringArray *self = (UtStringArray *)object;
  return self->data[index];
}

static char **ut_string_array_take_data(UtObject *object) {
  UtStringArray *self = (UtStringArray *)object;
  char **value = self->data;
  self->data = malloc(sizeof(char *) * 1);
  self->data[0] = NULL;
  self->data_length = 0;
  return value;
}

static void ut_string_array_insert(UtObject *object, size_t index,
                                   const char *value) {
  UtStringArray *self = (UtStringArray *)object;
  insert(self, index, value);
}

static void ut_string_array_init(UtObject *object) {
  UtStringArray *self = (UtStringArray *)object;
  self->data = malloc(sizeof(char *) * 1);
}

static bool ut_string_array_equal(UtObject *object, UtObject *other) {
  UtStringArray *self = (UtStringArray *)object;
  if (!ut_object_implements_string_list(other)) {
    return false;
  }
  if (self->data_length != ut_list_get_length(other)) {
    return false;
  }
  for (size_t i = 0; i < self->data_length; i++) {
    if (!ut_cstring_equal(self->data[i],
                          ut_string_list_get_element(other, i))) {
      return false;
    }
  }
  return true;
}

static void ut_string_array_cleanup(UtObject *object) {
  UtStringArray *self = (UtStringArray *)object;
  for (size_t i = 0; i < self->data_length; i++) {
    free(self->data[i]);
  }
  free(self->data);
}

static UtListInterface list_interface = {
    .is_mutable = true,
    .get_length = ut_string_array_get_length,
    .get_element = ut_string_array_get_element_object,
    .get_sublist = ut_string_array_get_sublist,
    .copy = ut_string_array_copy,
    .insert = ut_string_array_insert_object,
    .remove = ut_string_array_remove,
    .resize = ut_string_array_resize};

static UtStringListInterface string_list_interface = {
    .get_element = ut_string_array_get_element,
    .take_data = ut_string_array_take_data,
    .insert = ut_string_array_insert};

static UtObjectInterface object_interface = {
    .type_name = "UtStringArray",
    .init = ut_string_array_init,
    .to_string = _ut_list_to_string,
    .equal = ut_string_array_equal,
    .cleanup = ut_string_array_cleanup,
    .interfaces = {{&ut_list_id, &list_interface},
                   {&ut_string_list_id, &string_list_interface},
                   {NULL, NULL}}};

UtObject *ut_string_array_new() {
  return ut_object_new(sizeof(UtStringArray), &object_interface);
}

UtObject *ut_string_array_new_from_elements(const char *value, ...) {
  va_list ap;
  va_start(ap, value);
  UtObject *object = ut_string_array_new_from_va_elements(value, ap);
  va_end(ap);

  return object;
}

UtObject *ut_string_array_new_from_va_elements(const char *value, va_list ap) {
  UtObject *object = ut_string_array_new();
  UtStringArray *self = (UtStringArray *)object;

  insert(self, self->data_length, value);
  while (true) {
    const char *v = va_arg(ap, const char *);
    if (v == NULL) {
      break;
    }
    insert(self, self->data_length, v);
  }

  return object;
}

bool ut_object_is_string_array(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
