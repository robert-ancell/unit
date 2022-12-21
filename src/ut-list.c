#include <assert.h>
#include <stdlib.h>

#include "ut-list-private.h"
#include "ut.h"

int ut_list_id = 0;

UtObject *ut_list_new() { return ut_object_array_new(); }

UtObject *ut_list_new_from_elements(UtObject *item0, ...) {
  UtObject *object = ut_list_new();
  if (item0 == NULL) {
    return object;
  }

  va_list ap;
  va_start(ap, item0);
  ut_list_append(object, item0);
  while (true) {
    UtObject *value = va_arg(ap, UtObject *);
    if (value == NULL) {
      break;
    }
    ut_list_append(object, value);
  }
  va_end(ap);

  return object;
}

UtObject *ut_list_new_from_elements_take(UtObject *item0, ...) {
  UtObject *object = ut_list_new();
  if (item0 == NULL) {
    return object;
  }

  va_list ap;
  va_start(ap, item0);
  ut_list_append_take(object, item0);
  while (true) {
    UtObject *value = va_arg(ap, UtObject *);
    if (value == NULL) {
      break;
    }
    ut_list_append_take(object, value);
  }
  va_end(ap);

  return object;
}

size_t ut_list_get_length(UtObject *object) {
  UtListInterface *list_interface =
      ut_object_get_interface(object, &ut_list_id);
  assert(list_interface != NULL);
  return list_interface->get_length(object);
}

UtObject *ut_list_get_element(UtObject *object, size_t index) {
  UtListInterface *list_interface =
      ut_object_get_interface(object, &ut_list_id);
  assert(list_interface != NULL);
  return list_interface->get_element(object, index);
}

UtObject *ut_list_get_first(UtObject *object) {
  return ut_list_get_element(object, 0);
}

UtObject *ut_list_get_last(UtObject *object) {
  size_t length = ut_list_get_length(object);
  return length > 0 ? ut_list_get_element(object, length - 1) : NULL;
}

UtObject *ut_list_get_sublist(UtObject *object, size_t start, size_t count) {
  UtListInterface *list_interface =
      ut_object_get_interface(object, &ut_list_id);
  assert(list_interface != NULL);
  return list_interface->get_sublist(object, start, count);
}

UtObject *ut_list_copy(UtObject *object) {
  UtListInterface *list_interface =
      ut_object_get_interface(object, &ut_list_id);
  assert(list_interface != NULL);
  return list_interface->copy(object);
}

bool ut_list_is_mutable(UtObject *object) {
  UtListInterface *list_interface =
      ut_object_get_interface(object, &ut_list_id);
  assert(list_interface != NULL);
  return list_interface->is_mutable;
}

void ut_list_append(UtObject *object, UtObject *item) {
  size_t length = ut_list_get_length(object);
  ut_list_insert(object, length, item);
}

void ut_list_append_take(UtObject *object, UtObject *item) {
  ut_list_append(object, item);
  ut_object_unref(item);
}

void ut_list_append_list(UtObject *object, UtObject *list) {
  size_t length = ut_list_get_length(object);
  ut_list_insert_list(object, length, list);
}

void ut_list_prepend(UtObject *object, UtObject *item) {
  ut_list_insert(object, 0, item);
}

void ut_list_prepend_take(UtObject *object, UtObject *item) {
  ut_list_prepend(object, item);
  ut_object_unref(item);
}

void ut_list_prepend_list(UtObject *object, UtObject *list) {
  ut_list_insert_list(object, 0, list);
}

void ut_list_insert(UtObject *object, size_t index, UtObject *item) {
  UtListInterface *list_interface =
      ut_object_get_interface(object, &ut_list_id);
  assert(list_interface != NULL);
  assert(list_interface->is_mutable);
  list_interface->insert(object, index, item);
}

void ut_list_insert_take(UtObject *object, size_t index, UtObject *item) {
  ut_list_insert(object, index, item);
  ut_object_unref(item);
}

void ut_list_insert_list(UtObject *object, size_t index, UtObject *list) {
  UtListInterface *list_interface =
      ut_object_get_interface(object, &ut_list_id);
  assert(list_interface != NULL);
  assert(list_interface->is_mutable);
  if (list_interface->insert_list != NULL) {
    list_interface->insert_list(object, index, list);
  } else {
    size_t length = ut_list_get_length(list);
    for (size_t i = 0; i < length; i++) {
      ut_list_insert_take(object, index + i, ut_list_get_element(list, i));
    }
  }
}

void ut_list_remove(UtObject *object, size_t index, size_t count) {
  UtListInterface *list_interface =
      ut_object_get_interface(object, &ut_list_id);
  assert(list_interface != NULL);
  assert(list_interface->is_mutable);
  list_interface->remove(object, index, count);
}

void ut_list_clear(UtObject *object) { ut_list_resize(object, 0); }

void ut_list_resize(UtObject *object, size_t length) {
  UtListInterface *list_interface =
      ut_object_get_interface(object, &ut_list_id);
  assert(list_interface != NULL);
  assert(list_interface->is_mutable);
  list_interface->resize(object, length);
}

char *_ut_list_to_string(UtObject *object) {
  UtObjectRef string = ut_string_new("[");
  for (size_t i = 0; i < ut_list_get_length(object); i++) {
    UtObject *item = ut_list_get_element(object, i);

    if (i != 0) {
      ut_string_append(string, ", ");
    }

    ut_cstring_ref value_string = ut_object_to_string(item);
    ut_string_append(string, value_string);

    ut_object_unref(item);
  }
  ut_string_append(string, "]");

  return ut_string_take_text(string);
}

bool ut_object_implements_list(UtObject *object) {
  return ut_object_get_interface(object, &ut_list_id) != NULL;
}
