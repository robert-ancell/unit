#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;

  // Data being read.
  UtObject *data;

  // First error that occurred during encoding.
  UtObject *error;
} UtProtobufDecoder;

static void set_error(UtProtobufDecoder *self, const char *description) {
  if (self->error == NULL) {
    self->error = ut_protobuf_error_new(description);
  }
}

static void set_error_take(UtProtobufDecoder *self, char *description) {
  set_error(self, description);
  free(description);
}

static void set_insufficient_data_error(UtProtobufDecoder *self) {
  set_error(self, "Insufficient data");
}

static uint64_t read_varint(UtProtobufDecoder *self, size_t data_length,
                            size_t *offset) {
  uint64_t value = 0;
  size_t shift = 0;
  while (*offset < data_length) {
    uint8_t d = ut_uint8_list_get_element(self->data, *offset);
    (*offset)++;

    if ((shift == 63 && (d & 0x7e) != 0) || shift > 63) {
      set_error(self, "VARINT greater than 64 bit not supported");
      return 0;
    }

    value |= ((uint64_t)d & 0x7f) << shift;
    shift += 7;
    if ((d & 0x80) == 0) {
      return value;
    }
  }

  set_insufficient_data_error(self);
  return 0;
}

static uint32_t read_varint32(UtProtobufDecoder *self, size_t data_length,
                              size_t *offset) {
  uint64_t v = read_varint(self, data_length, offset);
  if (v > UINT32_MAX) {
    set_error(self, "VARINT too large for 32 bit value");
    return 0;
  }
  return v;
}

static uint64_t read_i64(UtProtobufDecoder *self, size_t data_length,
                         size_t *offset) {
  if (*offset + 8 > data_length) {
    set_insufficient_data_error(self);
    return 0;
  }

  uint64_t value = ut_uint8_list_get_uint64_le(self->data, *offset);
  *offset += 8;

  return value;
}

static uint32_t read_i32(UtProtobufDecoder *self, size_t data_length,
                         size_t *offset) {
  if (*offset + 4 > data_length) {
    set_insufficient_data_error(self);
    return 4;
  }

  uint32_t value = ut_uint8_list_get_uint32_le(self->data, *offset);
  *offset += 4;

  return value;
}

static int32_t decode_zig_zag32(uint32_t value) {
  uint32_t v = value >> 1;
  return (value & 0x1) != 0 ? -(1 + v) : v;
}

static int64_t decode_zig_zag64(uint64_t value) {
  uint64_t v = value >> 1;
  return (value & 0x1) != 0 ? -(1 + v) : v;
}

static int32_t read_int32(UtProtobufDecoder *self, size_t data_length,
                          size_t *offset) {
  int64_t v = read_varint(self, data_length, offset);
  if (v > INT32_MAX || v < INT32_MIN) {
    set_error(self, "VARINT too large for signed 32 bit value");
    return 0;
  }
  return v;
}

static int64_t read_int64(UtProtobufDecoder *self, size_t data_length,
                          size_t *offset) {
  return (int64_t)read_varint(self, data_length, offset);
}

static uint32_t read_uint32(UtProtobufDecoder *self, size_t data_length,
                            size_t *offset) {
  return read_varint32(self, data_length, offset);
}

static uint64_t read_uint64(UtProtobufDecoder *self, size_t data_length,
                            size_t *offset) {
  return read_varint(self, data_length, offset);
}

static int32_t read_sint32(UtProtobufDecoder *self, size_t data_length,
                           size_t *offset) {
  return decode_zig_zag32(read_varint32(self, data_length, offset));
}

static int64_t read_sint64(UtProtobufDecoder *self, size_t data_length,
                           size_t *offset) {
  return decode_zig_zag64(read_varint(self, data_length, offset));
}

static uint64_t read_fixed64(UtProtobufDecoder *self, size_t data_length,
                             size_t *offset) {
  return read_i64(self, data_length, offset);
}

static int64_t read_sfixed64(UtProtobufDecoder *self, size_t data_length,
                             size_t *offset) {
  return (int64_t)read_i64(self, data_length, offset);
}

static double read_double(UtProtobufDecoder *self, size_t data_length,
                          size_t *offset) {
  uint64_t value = read_i64(self, data_length, offset);
  double *v = (double *)&value;
  return *v;
}

static uint32_t read_fixed32(UtProtobufDecoder *self, size_t data_length,
                             size_t *offset) {
  return read_i32(self, data_length, offset);
}

static int32_t read_sfixed32(UtProtobufDecoder *self, size_t data_length,
                             size_t *offset) {
  return (int32_t)read_i32(self, data_length, offset);
}

static float read_float(UtProtobufDecoder *self, size_t data_length,
                        size_t *offset) {
  uint32_t value = read_i32(self, data_length, offset);
  float *v = (float *)&value;
  return *v;
}

static bool read_bool(UtProtobufDecoder *self, size_t data_length,
                      size_t *offset) {
  uint64_t v = read_varint(self, data_length, offset);
  if ((v & 0xfffffffffe) != 0) {
    set_error(self, "Invalid bool value");
    return false;
  }
  return v != 0;
}

static UtObject *get_type(UtObject *field) {
  UtObject *type = ut_protobuf_message_field_get_value_type(field);
  while (ut_object_is_protobuf_referenced_type(type)) {
    type = ut_protobuf_referenced_type_get_type(type);
  }
  return type;
}

static void read_varint_field(UtProtobufDecoder *self, size_t data_length,
                              size_t *offset, UtObject *message,
                              const char *name, UtObject *field) {
  if (field == NULL) {
    read_varint(self, data_length, offset);
    return;
  }

  bool is_repeated = ut_protobuf_message_field_get_type(field) ==
                     UT_PROTOBUF_MESSAGE_FIELD_TYPE_REPEATED;

  UtObject *type = get_type(field);
  UtObjectRef value = NULL;
  if (ut_object_is_protobuf_primitive_type(type)) {
    switch (ut_protobuf_primitive_type_get_type(type)) {
    case UT_PROTOBUF_PRIMITIVE_INT32:
      if (is_repeated) {
        ut_int32_list_append(ut_map_lookup_string(message, name),
                             read_int32(self, data_length, offset));
      } else {
        ut_map_insert_string_take(
            message, name, ut_int32_new(read_int32(self, data_length, offset)));
      }
      return;
    case UT_PROTOBUF_PRIMITIVE_INT64:
      if (is_repeated) {
        ut_int64_list_append(ut_map_lookup_string(message, name),
                             read_int64(self, data_length, offset));
      } else {
        ut_map_insert_string_take(
            message, name, ut_int64_new(read_int64(self, data_length, offset)));
      }
      return;
    case UT_PROTOBUF_PRIMITIVE_UINT32:
      if (is_repeated) {
        ut_uint32_list_append(ut_map_lookup_string(message, name),
                              read_uint32(self, data_length, offset));
      } else {
        ut_map_insert_string_take(
            message, name,
            ut_uint32_new(read_uint32(self, data_length, offset)));
      }
      return;
    case UT_PROTOBUF_PRIMITIVE_UINT64:
      if (is_repeated) {
        ut_uint64_list_append(ut_map_lookup_string(message, name),
                              read_uint64(self, data_length, offset));
      } else {
        ut_map_insert_string_take(
            message, name,
            ut_uint64_new(read_uint64(self, data_length, offset)));
      }
      return;
    case UT_PROTOBUF_PRIMITIVE_SINT32:
      if (is_repeated) {
        ut_int32_list_append(ut_map_lookup_string(message, name),
                             read_sint32(self, data_length, offset));
      } else {
        ut_map_insert_string_take(
            message, name,
            ut_int32_new(read_sint32(self, data_length, offset)));
      }
      return;
    case UT_PROTOBUF_PRIMITIVE_SINT64:
      if (is_repeated) {
        ut_int64_list_append(ut_map_lookup_string(message, name),
                             read_sint64(self, data_length, offset));
      } else {
        ut_map_insert_string_take(
            message, name,
            ut_int64_new(read_sint64(self, data_length, offset)));
      }
      return;
    case UT_PROTOBUF_PRIMITIVE_BOOL:
      if (is_repeated) {
        ut_boolean_list_append(ut_map_lookup_string(message, name),
                               read_bool(self, data_length, offset));
      } else {
        ut_map_insert_string_take(
            message, name,
            ut_boolean_new(read_bool(self, data_length, offset)));
      }
      return;
    default:
      break;
    }
  } else if (ut_object_is_protobuf_enum_type(type)) {
    int32_t enum_value = read_int32(self, data_length, offset);
    ut_cstring_ref enum_name =
        ut_protobuf_enum_type_lookup_name(type, enum_value);
    if (enum_name == NULL) {
      enum_name = ut_cstring_new_printf("%d", enum_value);
    }
    if (is_repeated) {
      ut_string_list_append(ut_map_lookup_string(message, name), enum_name);
    } else {
      ut_map_insert_string_take(message, name, ut_string_new(enum_name));
    }
    return;
  }

  set_error(self, "Received VARINT for non-VARINT field");
}

static void read_i64_field(UtProtobufDecoder *self, size_t data_length,
                           size_t *offset, UtObject *message, const char *name,
                           UtObject *field) {
  if (field == NULL) {
    read_i64(self, data_length, offset);
    return;
  }

  bool is_repeated = ut_protobuf_message_field_get_type(field) ==
                     UT_PROTOBUF_MESSAGE_FIELD_TYPE_REPEATED;

  UtObject *type = get_type(field);
  if (ut_object_is_protobuf_primitive_type(type)) {
    switch (ut_protobuf_primitive_type_get_type(type)) {
    case UT_PROTOBUF_PRIMITIVE_FIXED64:
      if (is_repeated) {
        ut_uint64_list_append(ut_map_lookup_string(message, name),
                              read_fixed64(self, data_length, offset));
      } else {
        ut_map_insert_string_take(
            message, name,
            ut_uint64_new(read_fixed64(self, data_length, offset)));
      }
      return;
    case UT_PROTOBUF_PRIMITIVE_SFIXED64:
      if (is_repeated) {
        ut_int64_list_append(ut_map_lookup_string(message, name),
                             read_sfixed64(self, data_length, offset));
      } else {
        ut_map_insert_string_take(
            message, name,
            ut_int64_new(read_sfixed64(self, data_length, offset)));
      }
      return;
    case UT_PROTOBUF_PRIMITIVE_DOUBLE:
      if (is_repeated) {
        ut_float64_list_append(ut_map_lookup_string(message, name),
                               read_double(self, data_length, offset));
      } else {
        ut_map_insert_string_take(
            message, name,
            ut_float64_new(read_double(self, data_length, offset)));
      }
      return;
    default:
      break;
    }
  }

  set_error(self, "Received I64 for non-I64 field");
}

static void process_string(UtProtobufDecoder *self, UtObject *data,
                           UtObject *message, const char *name,
                           bool is_repeated) {
  UtObjectRef string = ut_string_new_from_utf8(data);
  if (ut_object_implements_error(string)) {
    ut_cstring_ref error_description = ut_error_get_description(string);
    set_error_take(
        self, ut_cstring_new_printf("Invalid string: %s", error_description));
    return;
  }
  if (is_repeated) {
    ut_string_list_append(ut_map_lookup_string(message, name),
                          ut_string_get_text(string));
  } else {
    ut_map_insert_string(message, name, string);
  }
}

static void read_len_field(UtProtobufDecoder *self, size_t data_length,
                           size_t *offset, UtObject *message, const char *name,
                           UtObject *field) {
  int32_t length = read_int32(self, data_length, offset);
  if (length < 0) {
    set_error(self, "Negative LEN field");
    return;
  }
  if (*offset + length > data_length) {
    set_error(self, "Insufficient space for LEN data");
    return;
  }

  if (field == NULL) {
    *offset += length;
    return;
  }
  UtObjectRef data = ut_list_get_sublist(self->data, *offset, length);
  *offset += length;

  bool is_repeated = ut_protobuf_message_field_get_type(field) ==
                     UT_PROTOBUF_MESSAGE_FIELD_TYPE_REPEATED;

  UtObject *type = get_type(field);
  if (ut_object_is_protobuf_primitive_type(type)) {
    switch (ut_protobuf_primitive_type_get_type(type)) {
    case UT_PROTOBUF_PRIMITIVE_STRING:
      process_string(self, data, message, name, is_repeated);
      return;
    case UT_PROTOBUF_PRIMITIVE_BYTES:
      if (is_repeated) {
        ut_list_append(ut_map_lookup_string(message, name), data);
      } else {
        ut_map_insert_string(message, name, data);
      }
      return;
    default:
      break;
    }
  }

  set_error(self, "Received LEN for non-LEN field");
}

static void read_i32_field(UtProtobufDecoder *self, size_t data_length,
                           size_t *offset, UtObject *message, const char *name,
                           UtObject *field) {
  if (field == NULL) {
    read_i32(self, data_length, offset);
    return;
  }

  bool is_repeated = ut_protobuf_message_field_get_type(field) ==
                     UT_PROTOBUF_MESSAGE_FIELD_TYPE_REPEATED;

  UtObject *type = get_type(field);
  if (ut_object_is_protobuf_primitive_type(type)) {
    switch (ut_protobuf_primitive_type_get_type(type)) {
    case UT_PROTOBUF_PRIMITIVE_FIXED32:
      if (is_repeated) {
        ut_uint32_list_append(ut_map_lookup_string(message, name),
                              read_fixed32(self, data_length, offset));
      } else {
        ut_map_insert_string_take(
            message, name,
            ut_uint32_new(read_fixed32(self, data_length, offset)));
      }
      return;
    case UT_PROTOBUF_PRIMITIVE_SFIXED32:
      if (is_repeated) {
        ut_int32_list_append(ut_map_lookup_string(message, name),
                             read_sfixed32(self, data_length, offset));
      } else {
        ut_map_insert_string_take(
            message, name,
            ut_int32_new(read_sfixed32(self, data_length, offset)));
      }
      return;
    case UT_PROTOBUF_PRIMITIVE_FLOAT:
      if (is_repeated) {
        ut_float32_list_append(ut_map_lookup_string(message, name),
                               read_float(self, data_length, offset));
      } else {
        ut_map_insert_string_take(
            message, name,
            ut_float32_new(read_float(self, data_length, offset)));
      }
      return;
    default:
      break;
    }
  }

  set_error(self, "Received I32 for non-I32 field");
}

static UtObject *get_default_value(UtProtobufDecoder *self, UtObject *type) {
  if (ut_object_is_protobuf_primitive_type(type)) {
    switch (ut_protobuf_primitive_type_get_type(type)) {
    case UT_PROTOBUF_PRIMITIVE_DOUBLE:
      return ut_float64_new(0);
    case UT_PROTOBUF_PRIMITIVE_FLOAT:
      return ut_float32_new(0);
    case UT_PROTOBUF_PRIMITIVE_INT32:
    case UT_PROTOBUF_PRIMITIVE_SINT32:
    case UT_PROTOBUF_PRIMITIVE_SFIXED32:
      return ut_int32_new(0);
    case UT_PROTOBUF_PRIMITIVE_INT64:
    case UT_PROTOBUF_PRIMITIVE_SINT64:
    case UT_PROTOBUF_PRIMITIVE_SFIXED64:
      return ut_int64_new(0);
    case UT_PROTOBUF_PRIMITIVE_UINT32:
    case UT_PROTOBUF_PRIMITIVE_FIXED32:
      return ut_uint32_new(0);
    case UT_PROTOBUF_PRIMITIVE_UINT64:
    case UT_PROTOBUF_PRIMITIVE_FIXED64:
      return ut_uint64_new(0);
    case UT_PROTOBUF_PRIMITIVE_BOOL:
      return ut_boolean_new(false);
    case UT_PROTOBUF_PRIMITIVE_STRING:
      return ut_string_new("");
    case UT_PROTOBUF_PRIMITIVE_BYTES:
      return ut_uint8_list_new();
    }
  } else if (ut_object_is_protobuf_enum_type(type)) {
    UtObject *values_by_name =
        ut_protobuf_enum_type_lookup_values_by_name(type);
    UtObjectRef keys = ut_map_get_keys(values_by_name);
    if (ut_list_get_length(keys) == 0) {
      set_error(self, "Unable to determine default enum value");
      return NULL;
    }
    return ut_list_get_element(keys, 0);
  }

  set_error(self, "Unable to determine default value");
  return NULL;
}

static UtObject *get_repeated_field_initial_value(UtProtobufDecoder *self,
                                                  UtObject *type) {
  if (ut_object_is_protobuf_primitive_type(type)) {
    switch (ut_protobuf_primitive_type_get_type(type)) {
    case UT_PROTOBUF_PRIMITIVE_DOUBLE:
      return ut_float64_list_new();
    case UT_PROTOBUF_PRIMITIVE_FLOAT:
      return ut_float32_list_new();
    case UT_PROTOBUF_PRIMITIVE_INT32:
    case UT_PROTOBUF_PRIMITIVE_SINT32:
    case UT_PROTOBUF_PRIMITIVE_SFIXED32:
      return ut_int32_list_new();
    case UT_PROTOBUF_PRIMITIVE_INT64:
    case UT_PROTOBUF_PRIMITIVE_SINT64:
    case UT_PROTOBUF_PRIMITIVE_SFIXED64:
      return ut_int64_list_new();
    case UT_PROTOBUF_PRIMITIVE_UINT32:
    case UT_PROTOBUF_PRIMITIVE_FIXED32:
      return ut_uint32_list_new();
    case UT_PROTOBUF_PRIMITIVE_UINT64:
    case UT_PROTOBUF_PRIMITIVE_FIXED64:
      return ut_uint64_list_new();
    case UT_PROTOBUF_PRIMITIVE_BOOL:
      return ut_boolean_list_new();
    case UT_PROTOBUF_PRIMITIVE_STRING:
      return ut_string_list_new();
    case UT_PROTOBUF_PRIMITIVE_BYTES:
      return ut_object_list_new();
    }
  } else if (ut_object_is_protobuf_enum_type(type)) {
    return ut_string_list_new();
  } else if (ut_object_is_protobuf_message_type(type)) {
    return ut_object_list_new();
  }

  set_error(self, "Unable to determine list type for repeated field");
  return NULL;
}

static void ut_protobuf_decoder_cleanup(UtObject *object) {
  UtProtobufDecoder *self = (UtProtobufDecoder *)object;
  ut_object_unref(self->data);
  ut_object_unref(self->error);
}

static UtObjectInterface object_interface = {
    .type_name = "UtProtobufDecoder", .cleanup = ut_protobuf_decoder_cleanup};

UtObject *ut_protobuf_decoder_new(UtObject *data) {
  UtObject *object =
      ut_object_new(sizeof(UtProtobufDecoder), &object_interface);
  UtProtobufDecoder *self = (UtProtobufDecoder *)object;

  self->data = ut_object_ref(data);

  return object;
}

UtObject *ut_protobuf_decoder_decode_message(UtObject *object, UtObject *type) {
  assert(ut_object_is_protobuf_decoder(object));
  assert(ut_object_is_protobuf_message_type(type));
  UtProtobufDecoder *self = (UtProtobufDecoder *)object;

  UtObjectRef message = ut_map_new();

  // Set repeated fields to empty lists.
  UtObject *fields = ut_protobuf_message_type_get_fields(type);
  UtObjectRef field_items = ut_map_get_items(fields);
  size_t field_items_length = ut_list_get_length(field_items);
  for (size_t i = 0; i < field_items_length; i++) {
    UtObject *item = ut_object_list_get_element(field_items, i);
    UtObject *field = ut_map_item_get_value(item);

    if (ut_protobuf_message_field_get_type(field) ==
        UT_PROTOBUF_MESSAGE_FIELD_TYPE_REPEATED) {
      const char *name = ut_string_get_text(ut_map_item_get_key(item));
      UtObjectRef value =
          get_repeated_field_initial_value(self, get_type(field));
      if (value == NULL) {
        return ut_map_new();
      }
      ut_map_insert_string(message, name, value);
    }
  }

  size_t data_length = ut_list_get_length(self->data);
  size_t offset = 0;
  while (offset < data_length && self->error == NULL) {
    uint64_t tag = read_varint(self, data_length, &offset);

    uint8_t wire_type = tag & 0x7;
    uint32_t number = tag >> 3;
    const char *name = ut_protobuf_message_type_lookup_field_name(type, number);
    UtObject *field = ut_protobuf_message_type_lookup_field(type, name);

    switch (wire_type) {
    case 0:
      read_varint_field(self, data_length, &offset, message, name, field);
      break;
    case 1:
      read_i64_field(self, data_length, &offset, message, name, field);
      break;
    case 2:
      read_len_field(self, data_length, &offset, message, name, field);
      break;
    case 5:
      read_i32_field(self, data_length, &offset, message, name, field);
      break;
    default:
      set_error(self, "Unknown wire type");
      return ut_map_new();
    }
  }

  // Check for missing fields.
  for (size_t i = 0; i < field_items_length; i++) {
    UtObject *item = ut_object_list_get_element(field_items, i);
    const char *name = ut_string_get_text(ut_map_item_get_key(item));
    UtObject *field = ut_map_item_get_value(item);

    if (ut_protobuf_message_field_get_type(field) ==
        UT_PROTOBUF_MESSAGE_FIELD_TYPE_OPTIONAL) {
      if (ut_map_lookup_string(message, name) == NULL) {
        UtObjectRef default_value = get_default_value(self, get_type(field));
        if (default_value == NULL) {
          return ut_map_new();
        }
        ut_map_insert_string(message, name, default_value);
      }
      continue;
    }

    if (ut_map_lookup_string(message, name) == NULL) {
      set_error_take(self,
                     ut_cstring_new_printf("Missing required field %s", name));
      return ut_map_new();
    }
  }

  return ut_object_ref(message);
}

UtObject *ut_protobuf_decoder_get_error(UtObject *object) {
  assert(ut_object_is_protobuf_decoder(object));
  UtProtobufDecoder *self = (UtProtobufDecoder *)object;
  return self->error;
}

bool ut_object_is_protobuf_decoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
