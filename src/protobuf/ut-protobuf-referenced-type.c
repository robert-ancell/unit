#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  char *name;
  UtObject *type;
} UtProtobufReferencedType;

static char *ut_protobuf_referenced_type_to_string(UtObject *object) {
  UtProtobufReferencedType *self = (UtProtobufReferencedType *)object;
  return ut_cstring_new_printf("<UtProtobufReferencedType>(%s)", self->name);
}

static void ut_protobuf_referenced_type_cleanup(UtObject *object) {
  UtProtobufReferencedType *self = (UtProtobufReferencedType *)object;
  free(self->name);
  ut_object_unref(self->type);
}

static UtObjectInterface object_interface = {
    .type_name = "UtProtobufReferencedType",
    .to_string = ut_protobuf_referenced_type_to_string,
    .cleanup = ut_protobuf_referenced_type_cleanup};

UtObject *ut_protobuf_referenced_type_new(const char *name) {
  UtObject *object =
      ut_object_new(sizeof(UtProtobufReferencedType), &object_interface);
  UtProtobufReferencedType *self = (UtProtobufReferencedType *)object;

  self->name = ut_cstring_new(name);

  return object;
}

const char *ut_protobuf_referenced_type_get_name(UtObject *object) {
  assert(ut_object_is_protobuf_referenced_type(object));
  UtProtobufReferencedType *self = (UtProtobufReferencedType *)object;
  return self->name;
}

void ut_protobuf_referenced_type_set_type(UtObject *object, UtObject *type) {
  assert(ut_object_is_protobuf_referenced_type(object));
  UtProtobufReferencedType *self = (UtProtobufReferencedType *)object;
  self->type = ut_object_ref(type);
}

UtObject *ut_protobuf_referenced_type_get_type(UtObject *object) {
  assert(ut_object_is_protobuf_referenced_type(object));
  UtProtobufReferencedType *self = (UtProtobufReferencedType *)object;
  return self->type;
}

bool ut_object_is_protobuf_referenced_type(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
