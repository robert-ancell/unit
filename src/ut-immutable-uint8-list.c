#include <assert.h>
#include <stdlib.h>

#include "ut-immutable-uint8-list.h"
#include "ut-list.h"
#include "ut-uint8-list.h"

typedef struct {
  uint8_t *data;
  size_t data_length;
} UtImmutableUint8List;

const uint8_t *ut_immutable_uint8_list_get_data(UtObject *object) {
  UtImmutableUint8List *self = ut_object_get_data(object);
  return self->data;
}

static UtUint8ListFunctions uint8_list_functions = {
    .get_data = ut_immutable_uint8_list_get_data};

size_t ut_immutable_uint8_list_get_length(UtObject *object) {
  UtImmutableUint8List *self = ut_object_get_data(object);
  return self->data_length;
}

static UtListFunctions list_functions = {
    .get_length = ut_immutable_uint8_list_get_length};

static void ut_immutable_uint8_list_init(UtObject *object) {}

static void ut_immutable_uint8_list_cleanup(UtObject *object) {
  UtImmutableUint8List *self = ut_object_get_data(object);
  free(self->data);
}

static UtObjectFunctions object_functions = {
    .init = ut_immutable_uint8_list_init,
    .cleanup = ut_immutable_uint8_list_cleanup,
    {{&ut_uint8_list_id, &uint8_list_functions},
     {&ut_list_id, &list_functions}}};

UtObject *ut_immutable_uint8_list_new(const uint8_t *data, size_t data_length) {
  UtObject *object =
      ut_object_new(sizeof(UtImmutableUint8List), &object_functions);
  UtImmutableUint8List *self = ut_object_get_data(object);
  self->data = malloc(sizeof(uint8_t) * data_length);
  for (size_t i = 0; i < data_length; i++) {
    self->data[i] = data[i];
  }
  self->data_length = data_length;
  return object;
}

bool ut_object_is_immutable_uint8_list(UtObject *object) {
  return ut_object_is_type(object, &object_functions);
}