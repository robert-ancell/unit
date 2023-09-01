#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *values_by_name;
} UtProtobufEnumType;

static char *ut_protobuf_enum_type_to_string(UtObject *object) {
  UtProtobufEnumType *self = (UtProtobufEnumType *)object;
  ut_cstring_ref values_by_name_string =
      ut_object_to_string(self->values_by_name);
  return ut_cstring_new_printf("<UtProtobufEnumType>(%s)",
                               values_by_name_string);
}

static void ut_protobuf_enum_type_cleanup(UtObject *object) {
  UtProtobufEnumType *self = (UtProtobufEnumType *)object;
  ut_object_unref(self->values_by_name);
}

static UtObjectInterface object_interface = {
    .type_name = "UtProtobufEnumType",
    .to_string = ut_protobuf_enum_type_to_string,
    .cleanup = ut_protobuf_enum_type_cleanup};

UtObject *ut_protobuf_enum_type_new(UtObject *values_by_name) {
  UtObject *object =
      ut_object_new(sizeof(UtProtobufEnumType), &object_interface);
  UtProtobufEnumType *self = (UtProtobufEnumType *)object;

  self->values_by_name = ut_object_ref(values_by_name);

  return object;
}

UtObject *ut_protobuf_enum_type_new_take(UtObject *values_by_name) {
  UtObject *object = ut_protobuf_enum_type_new(values_by_name);
  ut_object_unref(values_by_name);
  return object;
}

bool ut_protobuf_enum_type_get_value(UtObject *object, const char *name,
                                     int32_t *value) {
  assert(ut_object_is_protobuf_enum_type(object));
  UtProtobufEnumType *self = (UtProtobufEnumType *)object;
  UtObject *value_object = ut_map_lookup_string(self->values_by_name, name);
  if (value_object == NULL) {
    return false;
  }

  *value = ut_int32_get_value(value_object);
  return true;
}

char *ut_protobuf_enum_type_get_name(UtObject *object, int32_t value) {
  assert(ut_object_is_protobuf_enum_type(object));
  UtProtobufEnumType *self = (UtProtobufEnumType *)object;

  UtObjectRef items = ut_map_get_items(self->values_by_name);
  size_t items_length = ut_list_get_length(items);
  for (size_t i = 0; i < items_length; i++) {
    UtObject *item = ut_object_list_get_element(items, i);
    UtObjectRef item_value = ut_map_item_get_value(item);
    if (ut_int32_get_value(item_value) == value) {
      UtObjectRef name = ut_map_item_get_key(item);
      return ut_cstring_new(ut_string_get_text(name));
    }
  }

  return NULL;
}

bool ut_object_is_protobuf_enum_type(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
