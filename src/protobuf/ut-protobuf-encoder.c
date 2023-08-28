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

static void encode_varint(UtProtobufEncoder *self, uint64_t value) {
  if (value <= 0x7f) {
    ut_uint8_list_append(self->buffer, value);
  } else if (value <= 0x3fff) {
    ut_uint8_list_append(self->buffer, value & 0x7f);
    ut_uint8_list_append(self->buffer, 0x80 | (value >> 7));
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
  ut_uint8_list_append(self->buffer, number << 3 | wire_type);
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

static bool check_type(UtProtobufEncoder *self, bool (*is_type)(UtObject *),
                       UtObject *value, const char *protobuf_type) {
  if (is_type(value)) {
    return true;
  }

  ut_cstring_ref description =
      ut_cstring_new_printf("Invalid type %s provided for %s",
                            ut_object_get_type_name(value), protobuf_type);
  set_error(self, description);
  return false;
}

static void encode_message(UtProtobufEncoder *self, UtObject *type,
                           UtObject *value) {
  UtObject *fields = ut_protobuf_message_type_get_fields(type);
  size_t fields_length = ut_list_get_length(fields);
  for (size_t i = 0; i < fields_length; i++) {
    UtObject *field = ut_object_list_get_element(fields, i);
    UtObject *field_type = ut_protobuf_message_field_get_type(field);
    uint32_t number = ut_protobuf_message_field_get_number(field);

    const char *name = ut_protobuf_message_field_get_name(field);
    UtObject *field_value = ut_map_lookup_string(value, name);
    if (field_value == NULL) {
      ut_cstring_ref description =
          ut_cstring_new_printf("Missing field %s", name);
      set_error(self, description);
      return;
    }

    if (ut_object_is_protobuf_primitive_type(field_type)) {
      switch (ut_protobuf_primitive_type_get_type(field_type)) {
      case UT_PROTOBUF_PRIMITIVE_DOUBLE:
        if (check_type(self, ut_object_is_float64, field_value, "double")) {
          encode_double(self, number, ut_float64_get_value(field_value));
        }
        break;
      case UT_PROTOBUF_PRIMITIVE_FLOAT:
        if (check_type(self, ut_object_is_float32, field_value, "float")) {
          encode_float(self, number, ut_float32_get_value(field_value));
        }
        break;
      case UT_PROTOBUF_PRIMITIVE_INT32:
        if (check_type(self, ut_object_is_int32, field_value, "int32")) {
          encode_int32(self, number, ut_int32_get_value(field_value));
        }
        break;
      case UT_PROTOBUF_PRIMITIVE_INT64:
        if (check_type(self, ut_object_is_int64, field_value, "int64")) {
          encode_int64(self, number, ut_int64_get_value(field_value));
        }
        break;
      case UT_PROTOBUF_PRIMITIVE_UINT32:
        if (check_type(self, ut_object_is_uint32, field_value, "uint32")) {
          encode_uint32(self, number, ut_uint32_get_value(field_value));
        }
        break;
      case UT_PROTOBUF_PRIMITIVE_UINT64:
        if (check_type(self, ut_object_is_uint64, field_value, "uint64")) {
          encode_uint64(self, number, ut_uint64_get_value(field_value));
        }
        break;
      case UT_PROTOBUF_PRIMITIVE_SINT32:
        if (check_type(self, ut_object_is_int32, field_value, "sint32")) {
          encode_sint32(self, number, ut_int32_get_value(field_value));
        }
        break;
      case UT_PROTOBUF_PRIMITIVE_SINT64:
        if (check_type(self, ut_object_is_int64, field_value, "sint64")) {
          encode_sint64(self, number, ut_int64_get_value(field_value));
        }
        break;
      case UT_PROTOBUF_PRIMITIVE_FIXED32:
        if (check_type(self, ut_object_is_uint32, field_value, "fixed32")) {
          encode_fixed32(self, number, ut_uint32_get_value(field_value));
        }
        break;
      case UT_PROTOBUF_PRIMITIVE_FIXED64:
        if (check_type(self, ut_object_is_uint64, field_value, "fixed64")) {
          encode_fixed64(self, number, ut_uint64_get_value(field_value));
        }
        break;
      case UT_PROTOBUF_PRIMITIVE_SFIXED32:
        if (check_type(self, ut_object_is_int32, field_value, "sfixed32")) {
          encode_sfixed32(self, number, ut_int32_get_value(field_value));
        }
        break;
      case UT_PROTOBUF_PRIMITIVE_SFIXED64:
        if (check_type(self, ut_object_is_int64, field_value, "sfixed64")) {
          encode_sfixed64(self, number, ut_int64_get_value(field_value));
        }
        break;
      case UT_PROTOBUF_PRIMITIVE_BOOL:
        if (check_type(self, ut_object_is_boolean, field_value, "bool")) {
          encode_bool(self, number, ut_boolean_get_value(field_value));
        }
        break;
      case UT_PROTOBUF_PRIMITIVE_STRING:
        if (check_type(self, ut_object_implements_string, field_value,
                       "string")) {
          encode_string(self, number, ut_string_get_text(field_value));
        }
        break;
      case UT_PROTOBUF_PRIMITIVE_BYTES:
        if (check_type(self, ut_object_implements_uint8_list, field_value,
                       "bytes")) {
          encode_bytes(self, number, field_value);
        }
        break;
      }
    } else if (ut_object_is_protobuf_message_type(field_type)) {
      encode_message(self, field_type, value);
    } else {
      ut_cstring_ref type_string = ut_object_to_string(field_type);
      ut_cstring_ref description =
          ut_cstring_new_printf("Unknown Protobuf type %s", type_string);
      set_error(self, description);
      return;
    }
  }
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
