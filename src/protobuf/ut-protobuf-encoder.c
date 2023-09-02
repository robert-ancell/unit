#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;

  // Data being written.
  UtObject *buffer;

  // First error that occurred during encoding.
  UtObject *error;
} UtProtobufEncoder;

static void set_error(UtProtobufEncoder *self, const char *description) {
  if (self->error == NULL) {
    self->error = ut_protobuf_error_new(description);
  }
}

static void set_error_take(UtProtobufEncoder *self, char *description) {
  set_error(self, description);
  free(description);
}

static void encode_varint(UtProtobufEncoder *self, uint64_t value) {
  if (value <= 0x7f) {
    ut_uint8_list_append(self->buffer, value);
  } else if (value <= 0x3fff) {
    ut_uint8_list_append(self->buffer, 0x80 | (value & 0x7f));
    ut_uint8_list_append(self->buffer, value >> 7);
  } else if (value <= 0x1fffff) {
    ut_uint8_list_append(self->buffer, 0x80 | (value & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 7) & 0x7f));
    ut_uint8_list_append(self->buffer, value >> 14);
  } else if (value <= 0xfffffff) {
    ut_uint8_list_append(self->buffer, 0x80 | (value & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 7) & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 14) & 0x7f));
    ut_uint8_list_append(self->buffer, value >> 21);
  } else if (value <= 0x7ffffffff) {
    ut_uint8_list_append(self->buffer, 0x80 | (value & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 7) & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 14) & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 21) & 0x7f));
    ut_uint8_list_append(self->buffer, value >> 28);
  } else if (value <= 0x3ffffffffff) {
    ut_uint8_list_append(self->buffer, 0x80 | (value & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 7) & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 14) & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 21) & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 28) & 0x7f));
    ut_uint8_list_append(self->buffer, value >> 35);
  } else if (value <= 0x1ffffffffffff) {
    ut_uint8_list_append(self->buffer, 0x80 | (value & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 7) & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 14) & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 21) & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 28) & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 35) & 0x7f));
    ut_uint8_list_append(self->buffer, value >> 42);
  } else if (value <= 0xffffffffffffff) {
    ut_uint8_list_append(self->buffer, 0x80 | (value & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 7) & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 14) & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 21) & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 28) & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 35) & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 42) & 0x7f));
    ut_uint8_list_append(self->buffer, value >> 49);
  } else if (value <= 0x7fffffffffffffff) {
    ut_uint8_list_append(self->buffer, 0x80 | (value & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 7) & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 14) & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 21) & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 28) & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 35) & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 42) & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 49) & 0x7f));
    ut_uint8_list_append(self->buffer, value >> 56);
  } else {
    ut_uint8_list_append(self->buffer, 0x80 | (value & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 7) & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 14) & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 21) & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 28) & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 35) & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 42) & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 49) & 0x7f));
    ut_uint8_list_append(self->buffer, 0x80 | ((value >> 56) & 0x7f));
    ut_uint8_list_append(self->buffer, value >> 63);
  }
}

static void encode_tag(UtProtobufEncoder *self, uint32_t number,
                       uint32_t wire_type) {
  if (number == 0 || number > 0x1fffffff) {
    set_error(self, "Invalid tag number");
    return;
  }
  encode_varint(self, number << 3 | wire_type);
}

static void encode_varint_record(UtProtobufEncoder *self, uint32_t number,
                                 uint64_t value) {
  encode_tag(self, number, 0);
  encode_varint(self, value);
}

static void encode_i64_record(UtProtobufEncoder *self, uint32_t number,
                              uint64_t value) {
  encode_tag(self, number, 1);
  ut_uint8_list_append_uint64_le(self->buffer, value);
}

static void encode_len_record(UtProtobufEncoder *self, uint32_t number,
                              UtObject *data) {
  encode_tag(self, number, 2);
  encode_varint(self, ut_list_get_length(data));
  ut_list_append_list(self->buffer, data);
}

static void encode_i32_record(UtProtobufEncoder *self, uint32_t number,
                              uint32_t value) {
  encode_tag(self, number, 5);
  ut_uint8_list_append_uint32_le(self->buffer, value);
}

static uint32_t encode_zig_zag32(int32_t value) {
  return (value << 1) ^ (value >> 31);
}

static uint64_t encode_zig_zag64(int64_t value) {
  return (value << 1) ^ (value >> 63);
}

static void encode_uint32(UtProtobufEncoder *self, uint32_t number,
                          uint32_t value) {
  encode_varint_record(self, number, value);
}

static void encode_int32(UtProtobufEncoder *self, uint32_t number,
                         int32_t value) {
  encode_varint_record(self, number, (uint64_t)value);
}

static void encode_uint64(UtProtobufEncoder *self, uint32_t number,
                          uint64_t value) {
  encode_varint_record(self, number, value);
}

static void encode_int64(UtProtobufEncoder *self, uint32_t number,
                         int64_t value) {
  encode_varint_record(self, number, (uint64_t)value);
}

static void encode_sint32(UtProtobufEncoder *self, uint32_t number,
                          int32_t value) {
  encode_varint_record(self, number, encode_zig_zag32(value));
}

static void encode_sint64(UtProtobufEncoder *self, uint32_t number,
                          int64_t value) {
  encode_varint_record(self, number, encode_zig_zag64(value));
}

static void encode_fixed32(UtProtobufEncoder *self, uint32_t number,
                           uint32_t value) {
  encode_i32_record(self, number, value);
}

static void encode_fixed64(UtProtobufEncoder *self, uint64_t number,
                           uint64_t value) {
  encode_i64_record(self, number, value);
}

static void encode_sfixed32(UtProtobufEncoder *self, uint32_t number,
                            int32_t value) {
  encode_i32_record(self, number, (uint32_t)value);
}

static void encode_sfixed64(UtProtobufEncoder *self, uint64_t number,
                            int64_t value) {
  encode_i64_record(self, number, (uint64_t)value);
}

static void encode_bool(UtProtobufEncoder *self, uint32_t number, bool value) {
  encode_varint_record(self, number, value ? 1 : 0);
}

static void encode_float(UtProtobufEncoder *self, uint32_t number,
                         float value) {
  uint32_t *v = (uint32_t *)&value;
  encode_i32_record(self, number, *v);
}

static void encode_double(UtProtobufEncoder *self, uint32_t number,
                          double value) {
  uint64_t *v = (uint64_t *)&value;
  encode_i64_record(self, number, *v);
}

static void encode_string(UtProtobufEncoder *self, uint32_t number,
                          const char *value) {
  UtObjectRef string_value = ut_string_new_constant(value);
  UtObjectRef utf8 = ut_string_get_utf8(string_value);
  encode_len_record(self, number, utf8);
}

static void encode_bytes(UtProtobufEncoder *self, uint32_t number,
                         UtObject *value) {
  encode_len_record(self, number, value);
}

static void encode_enum(UtProtobufEncoder *self, uint32_t number,
                        UtObject *type, const char *value) {
  int32_t enum_value;
  if (!ut_protobuf_enum_type_get_value(type, value, &enum_value)) {
    set_error_take(self, ut_cstring_new_printf("Unknown enum value %s", value));
    return;
  }
  encode_int32(self, number, enum_value);
}

static bool check_type(UtProtobufEncoder *self, bool (*is_type)(UtObject *),
                       UtObject *value, const char *protobuf_type) {
  if (is_type(value)) {
    return true;
  }

  set_error_take(self, ut_cstring_new_printf("Invalid type %s provided for %s",
                                             ut_object_get_type_name(value),
                                             protobuf_type));
  return false;
}

static bool encode_message(UtProtobufEncoder *self, UtObject *type,
                           UtObject *value);

static bool encode_repeated_field(UtProtobufEncoder *self, uint32_t number,
                                  UtObject *type, UtObject *value) {
  size_t value_length;
  if (ut_object_is_protobuf_primitive_type(type)) {
    switch (ut_protobuf_primitive_type_get_type(type)) {
    case UT_PROTOBUF_PRIMITIVE_DOUBLE:
      if (!check_type(self, ut_object_implements_float64_list, value,
                      "double")) {
        return false;
      }
      value_length = ut_list_get_length(value);
      for (size_t i = 0; i < value_length; i++) {
        encode_double(self, number, ut_float64_list_get_element(value, i));
      }
      return true;
    case UT_PROTOBUF_PRIMITIVE_FLOAT:
      if (!check_type(self, ut_object_implements_float32_list, value,
                      "float")) {
        return false;
      }
      value_length = ut_list_get_length(value);
      for (size_t i = 0; i < value_length; i++) {
        encode_float(self, number, ut_float32_list_get_element(value, i));
      }
      return true;
    case UT_PROTOBUF_PRIMITIVE_INT32:
      if (!check_type(self, ut_object_implements_int32_list, value, "int32")) {
        return false;
      }
      value_length = ut_list_get_length(value);
      for (size_t i = 0; i < value_length; i++) {
        encode_int32(self, number, ut_int32_list_get_element(value, i));
      }
      return true;
    case UT_PROTOBUF_PRIMITIVE_INT64:
      if (!check_type(self, ut_object_implements_int64_list, value, "int64")) {
        return false;
      }
      value_length = ut_list_get_length(value);
      for (size_t i = 0; i < value_length; i++) {
        encode_int64(self, number, ut_int64_list_get_element(value, i));
      }
      return true;
    case UT_PROTOBUF_PRIMITIVE_UINT32:
      if (!check_type(self, ut_object_implements_uint32_list, value,
                      "uint32")) {
        return false;
      }
      value_length = ut_list_get_length(value);
      for (size_t i = 0; i < value_length; i++) {
        encode_uint32(self, number, ut_uint32_list_get_element(value, i));
      }
      return true;
    case UT_PROTOBUF_PRIMITIVE_UINT64:
      if (!check_type(self, ut_object_implements_uint64_list, value,
                      "uint64")) {
        return false;
      }
      value_length = ut_list_get_length(value);
      for (size_t i = 0; i < value_length; i++) {
        encode_uint64(self, number, ut_uint64_list_get_element(value, i));
      }
      return true;
    case UT_PROTOBUF_PRIMITIVE_SINT32:
      if (!check_type(self, ut_object_implements_int32_list, value, "sint32")) {
        return false;
      }
      value_length = ut_list_get_length(value);
      for (size_t i = 0; i < value_length; i++) {
        encode_sint32(self, number, ut_int32_list_get_element(value, i));
      }
      return true;
    case UT_PROTOBUF_PRIMITIVE_SINT64:
      if (!check_type(self, ut_object_implements_int64_list, value, "sint64")) {
        return false;
      }
      value_length = ut_list_get_length(value);
      for (size_t i = 0; i < value_length; i++) {
        encode_sint64(self, number, ut_int64_list_get_element(value, i));
      }
      return true;
    case UT_PROTOBUF_PRIMITIVE_FIXED32:
      if (!check_type(self, ut_object_implements_uint32_list, value,
                      "fixed32")) {
        return false;
      }
      value_length = ut_list_get_length(value);
      for (size_t i = 0; i < value_length; i++) {
        encode_fixed32(self, number, ut_uint32_list_get_element(value, i));
      }
      return true;
    case UT_PROTOBUF_PRIMITIVE_FIXED64:
      if (!check_type(self, ut_object_implements_uint64_list, value,
                      "fixed64")) {
        return false;
      }
      value_length = ut_list_get_length(value);
      for (size_t i = 0; i < value_length; i++) {
        encode_fixed64(self, number, ut_uint64_list_get_element(value, i));
      }
      return true;
    case UT_PROTOBUF_PRIMITIVE_SFIXED32:
      if (!check_type(self, ut_object_implements_int32_list, value,
                      "sfixed32")) {
        return false;
      }
      value_length = ut_list_get_length(value);
      for (size_t i = 0; i < value_length; i++) {
        encode_sfixed32(self, number, ut_int32_list_get_element(value, i));
      }
      return true;
    case UT_PROTOBUF_PRIMITIVE_SFIXED64:
      if (!check_type(self, ut_object_implements_int64_list, value,
                      "sfixed64")) {
        return false;
      }
      value_length = ut_list_get_length(value);
      for (size_t i = 0; i < value_length; i++) {
        encode_sfixed64(self, number, ut_int64_list_get_element(value, i));
      }
      return true;
    case UT_PROTOBUF_PRIMITIVE_BOOL:
      if (!check_type(self, ut_object_implements_boolean_list, value, "bool")) {
        return false;
      }
      value_length = ut_list_get_length(value);
      for (size_t i = 0; i < value_length; i++) {
        encode_bool(self, number, ut_boolean_list_get_element(value, i));
      }
      return true;
    case UT_PROTOBUF_PRIMITIVE_STRING:
      if (!check_type(self, ut_object_implements_string_list, value,
                      "string")) {
        return false;
      }
      value_length = ut_list_get_length(value);
      for (size_t i = 0; i < value_length; i++) {
        encode_string(self, number, ut_string_list_get_element(value, i));
      }
      return true;
    case UT_PROTOBUF_PRIMITIVE_BYTES:
      if (!check_type(self, ut_object_implements_list, value, "bytes")) {
        return false;
      }
      value_length = ut_list_get_length(value);
      for (size_t i = 0; i < value_length; i++) {
        UtObject *data = ut_object_list_get_element(value, i);
        encode_bytes(self, number, data);
      }
      return true;
    }
  } else if (ut_object_is_protobuf_enum_type(type)) {
    if (!check_type(self, ut_object_implements_string_list, value, "enum")) {
      return false;
    }
    value_length = ut_list_get_length(value);
    for (size_t i = 0; i < value_length; i++) {
      encode_enum(self, number, type, ut_string_list_get_element(value, i));
    }
    return true;
  } else if (ut_object_is_protobuf_message_type(type)) {
    if (!check_type(self, ut_object_implements_map, value, "message")) {
      return false;
    }
    value_length = ut_list_get_length(value);
    for (size_t i = 0; i < value_length; i++) {
      UtObject *message = ut_object_list_get_element(value, i);
      if (!encode_message(self, type, message)) {
        return false;
      }
    }
    return true;
  }

  ut_cstring_ref type_string = ut_object_to_string(type);
  set_error_take(self, ut_cstring_new_printf(
                           "Unknown Protobuf repeated type %s", type_string));

  return false;
}

static bool encode_single_field(UtProtobufEncoder *self, uint32_t number,
                                UtObject *type, UtObject *value) {
  if (ut_object_is_protobuf_primitive_type(type)) {
    switch (ut_protobuf_primitive_type_get_type(type)) {
    case UT_PROTOBUF_PRIMITIVE_DOUBLE:
      if (!check_type(self, ut_object_is_float64, value, "double")) {
        return false;
      }
      encode_double(self, number, ut_float64_get_value(value));
      return true;
    case UT_PROTOBUF_PRIMITIVE_FLOAT:
      if (!check_type(self, ut_object_is_float32, value, "float")) {
        return false;
      }
      encode_float(self, number, ut_float32_get_value(value));
      return true;
    case UT_PROTOBUF_PRIMITIVE_INT32:
      if (!check_type(self, ut_object_is_int32, value, "int32")) {
        return false;
      }
      encode_int32(self, number, ut_int32_get_value(value));
      return true;
    case UT_PROTOBUF_PRIMITIVE_INT64:
      if (!check_type(self, ut_object_is_int64, value, "int64")) {
        return false;
      }
      encode_int64(self, number, ut_int64_get_value(value));
      return true;
    case UT_PROTOBUF_PRIMITIVE_UINT32:
      if (!check_type(self, ut_object_is_uint32, value, "uint32")) {
        return false;
      }
      encode_uint32(self, number, ut_uint32_get_value(value));
      return true;
    case UT_PROTOBUF_PRIMITIVE_UINT64:
      if (!check_type(self, ut_object_is_uint64, value, "uint64")) {
        return false;
      }
      encode_uint64(self, number, ut_uint64_get_value(value));
      return true;
    case UT_PROTOBUF_PRIMITIVE_SINT32:
      if (!check_type(self, ut_object_is_int32, value, "sint32")) {
        return false;
      }
      encode_sint32(self, number, ut_int32_get_value(value));
      return true;
    case UT_PROTOBUF_PRIMITIVE_SINT64:
      if (!check_type(self, ut_object_is_int64, value, "sint64")) {
        return false;
      }
      encode_sint64(self, number, ut_int64_get_value(value));
      return true;
    case UT_PROTOBUF_PRIMITIVE_FIXED32:
      if (!check_type(self, ut_object_is_uint32, value, "fixed32")) {
        return false;
      }
      encode_fixed32(self, number, ut_uint32_get_value(value));
      return true;
    case UT_PROTOBUF_PRIMITIVE_FIXED64:
      if (!check_type(self, ut_object_is_uint64, value, "fixed64")) {
        return false;
      }
      encode_fixed64(self, number, ut_uint64_get_value(value));
      return true;
    case UT_PROTOBUF_PRIMITIVE_SFIXED32:
      if (!check_type(self, ut_object_is_int32, value, "sfixed32")) {
        return false;
      }
      encode_sfixed32(self, number, ut_int32_get_value(value));
      return true;
    case UT_PROTOBUF_PRIMITIVE_SFIXED64:
      if (!check_type(self, ut_object_is_int64, value, "sfixed64")) {
        return false;
      }
      encode_sfixed64(self, number, ut_int64_get_value(value));
      return true;
    case UT_PROTOBUF_PRIMITIVE_BOOL:
      if (!check_type(self, ut_object_is_boolean, value, "bool")) {
        return false;
      }
      encode_bool(self, number, ut_boolean_get_value(value));
      return true;
    case UT_PROTOBUF_PRIMITIVE_STRING:
      if (!check_type(self, ut_object_implements_string, value, "string")) {
        return false;
      }
      encode_string(self, number, ut_string_get_text(value));
      return true;
    case UT_PROTOBUF_PRIMITIVE_BYTES:
      if (!check_type(self, ut_object_implements_uint8_list, value, "bytes")) {
        return false;
      }
      encode_bytes(self, number, value);
      return true;
    }
  } else if (ut_object_is_protobuf_enum_type(type)) {
    if (!check_type(self, ut_object_implements_string, value, "enum")) {
      return false;
    }
    encode_enum(self, number, type, ut_string_get_text(value));
    return true;
  } else if (ut_object_is_protobuf_message_type(type)) {
    if (!check_type(self, ut_object_implements_map, value, "message")) {
      return false;
    }
    encode_message(self, type, value);
    return true;
  }

  ut_cstring_ref type_string = ut_object_to_string(type);
  set_error_take(
      self, ut_cstring_new_printf("Unknown Protobuf type %s", type_string));
  return false;
}

static bool encode_message(UtProtobufEncoder *self, UtObject *type,
                           UtObject *value) {
  UtObject *fields = ut_protobuf_message_type_get_fields(type);
  UtObjectRef field_items = ut_map_get_items(fields);
  size_t field_items_length = ut_list_get_length(field_items);
  for (size_t i = 0; i < field_items_length; i++) {
    UtObject *item = ut_object_list_get_element(field_items, i);
    UtObject *field = ut_map_item_get_value(item);
    UtProtobufMessageFieldType field_type =
        ut_protobuf_message_field_get_type(field);
    UtObject *field_value_type =
        ut_protobuf_message_field_get_value_type(field);
    uint32_t number = ut_protobuf_message_field_get_number(field);

    while (ut_object_is_protobuf_referenced_type(field_value_type)) {
      field_value_type = ut_protobuf_referenced_type_get_type(field_value_type);
    }

    const char *name = ut_string_get_text(ut_map_item_get_key(item));
    UtObject *field_value = ut_map_lookup_string(value, name);
    if (field_value == NULL) {
      if (field_type == UT_PROTOBUF_MESSAGE_FIELD_TYPE_OPTIONAL) {
        continue;
      }

      set_error_take(self, ut_cstring_new_printf("Missing field %s", name));
      return false;
    }

    if (field_type == UT_PROTOBUF_MESSAGE_FIELD_TYPE_REPEATED) {
      if (!encode_repeated_field(self, number, field_value_type, field_value)) {
        return false;
      }
      continue;
    }

    if (!encode_single_field(self, number, field_value_type, field_value)) {
      return false;
    }
  }

  return true;
}

static void ut_protobuf_encoder_init(UtObject *object) {
  UtProtobufEncoder *self = (UtProtobufEncoder *)object;
  self->buffer = ut_uint8_array_new();
}

static void ut_protobuf_encoder_cleanup(UtObject *object) {
  UtProtobufEncoder *self = (UtProtobufEncoder *)object;
  ut_object_unref(self->buffer);
  ut_object_unref(self->error);
}

static UtObjectInterface object_interface = {.type_name = "UtProtobufEncoder",
                                             .init = ut_protobuf_encoder_init,
                                             .cleanup =
                                                 ut_protobuf_encoder_cleanup};

UtObject *ut_protobuf_encoder_new() {
  return ut_object_new(sizeof(UtProtobufEncoder), &object_interface);
}

void ut_protobuf_encoder_encode_message(UtObject *object, UtObject *type,
                                        UtObject *message) {
  assert(ut_object_is_protobuf_encoder(object));
  assert(ut_object_is_protobuf_message_type(type));
  UtProtobufEncoder *self = (UtProtobufEncoder *)object;

  encode_message(self, type, message);
}

UtObject *ut_protobuf_encoder_get_data(UtObject *object) {
  assert(ut_object_is_protobuf_encoder(object));
  UtProtobufEncoder *self = (UtProtobufEncoder *)object;
  return self->buffer;
}

UtObject *ut_protobuf_encoder_get_error(UtObject *object) {
  assert(ut_object_is_protobuf_encoder(object));
  UtProtobufEncoder *self = (UtProtobufEncoder *)object;
  return self->error;
}

bool ut_object_is_protobuf_encoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
