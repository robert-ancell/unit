#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *type;
  char *name;
  uint32_t number;
} UtProtobufMessageField;

static char *ut_protobuf_message_field_to_string(UtObject *object) {
  UtProtobufMessageField *self = (UtProtobufMessageField *)object;
  ut_cstring_ref type_string = ut_object_to_string(self->type);
  return ut_cstring_new_printf("<UtProtobufMessageField>(%s, %s, %u)",
                               type_string, self->name, self->number);
}

static void ut_protobuf_message_field_cleanup(UtObject *object) {
  UtProtobufMessageField *self = (UtProtobufMessageField *)object;
  ut_object_unref(self->type);
  free(self->name);
}

static UtObjectInterface object_interface = {
    .type_name = "UtProtobufMessageField",
    .to_string = ut_protobuf_message_field_to_string,
    .cleanup = ut_protobuf_message_field_cleanup};

UtObject *ut_protobuf_message_field_new(UtObject *type, const char *name,
                                        uint32_t number) {
  UtObject *object =
      ut_object_new(sizeof(UtProtobufMessageField), &object_interface);
  UtProtobufMessageField *self = (UtProtobufMessageField *)object;

  self->type = ut_object_ref(type);
  self->name = ut_cstring_new(name);
  self->number = number;

  return object;
}

UtObject *ut_protobuf_message_field_new_take(UtObject *type, const char *name,
                                             uint32_t number) {
  UtObject *object = ut_protobuf_message_field_new(type, name, number);
  ut_object_unref(type);
  return object;
}

UtObject *ut_protobuf_message_field_get_type(UtObject *object) {
  assert(ut_object_is_protobuf_message_field(object));
  UtProtobufMessageField *self = (UtProtobufMessageField *)object;
  return self->type;
}

const char *ut_protobuf_message_field_get_name(UtObject *object) {
  assert(ut_object_is_protobuf_message_field(object));
  UtProtobufMessageField *self = (UtProtobufMessageField *)object;
  return self->name;
}

uint32_t ut_protobuf_message_field_get_number(UtObject *object) {
  assert(ut_object_is_protobuf_message_field(object));
  UtProtobufMessageField *self = (UtProtobufMessageField *)object;
  return self->number;
}

bool ut_object_is_protobuf_message_field(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
