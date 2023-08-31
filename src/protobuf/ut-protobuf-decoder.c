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

static void read_varint_field(UtProtobufDecoder *self, size_t data_length,
                              size_t *offset, UtObject *message,
                              UtObject *field) {
  if (field == NULL) {
    read_varint(self, data_length, offset);
    return;
  }

  const char *name = ut_protobuf_message_field_get_name(field);
  UtObject *type = ut_protobuf_message_field_get_type(field);
  if (ut_object_is_protobuf_primitive_type(type)) {
    switch (ut_protobuf_primitive_type_get_type(type)) {
    case UT_PROTOBUF_PRIMITIVE_INT32:
      ut_map_insert_string_take(
          message, name, ut_int32_new(read_int32(self, data_length, offset)));
      return;
    case UT_PROTOBUF_PRIMITIVE_INT64:
      ut_map_insert_string_take(
          message, name, ut_int64_new(read_int64(self, data_length, offset)));
      return;
    case UT_PROTOBUF_PRIMITIVE_UINT32:
      ut_map_insert_string_take(
          message, name, ut_uint32_new(read_uint32(self, data_length, offset)));
      return;
    case UT_PROTOBUF_PRIMITIVE_UINT64:
      ut_map_insert_string_take(
          message, name, ut_uint64_new(read_uint64(self, data_length, offset)));
      return;
    case UT_PROTOBUF_PRIMITIVE_SINT32:
      ut_map_insert_string_take(
          message, name, ut_int32_new(read_sint32(self, data_length, offset)));
      return;
    case UT_PROTOBUF_PRIMITIVE_SINT64:
      ut_map_insert_string_take(
          message, name, ut_int64_new(read_sint64(self, data_length, offset)));
      return;
    case UT_PROTOBUF_PRIMITIVE_BOOL:
      ut_map_insert_string_take(
          message, name, ut_boolean_new(read_bool(self, data_length, offset)));
      return;
    default:
      break;
    }
  }

  set_error(self, "Received VARINT for non-VARINT field");
}

static void read_i64_field(UtProtobufDecoder *self, size_t data_length,
                           size_t *offset, UtObject *message, UtObject *field) {
  if (field == NULL) {
    read_i64(self, data_length, offset);
    return;
  }

  const char *name = ut_protobuf_message_field_get_name(field);
  UtObject *type = ut_protobuf_message_field_get_type(field);
  if (ut_object_is_protobuf_primitive_type(type)) {
    switch (ut_protobuf_primitive_type_get_type(type)) {
    case UT_PROTOBUF_PRIMITIVE_FIXED64:
      ut_map_insert_string_take(
          message, name,
          ut_uint64_new(read_fixed64(self, data_length, offset)));
      return;
    case UT_PROTOBUF_PRIMITIVE_SFIXED64:
      ut_map_insert_string_take(
          message, name,
          ut_int64_new(read_sfixed64(self, data_length, offset)));
      return;
    case UT_PROTOBUF_PRIMITIVE_DOUBLE:
      ut_map_insert_string_take(
          message, name,
          ut_float64_new(read_double(self, data_length, offset)));
      return;
    default:
      break;
    }
  }

  set_error(self, "Received I64 for non-I64 field");
}

static void process_string(UtProtobufDecoder *self, UtObject *data,
                           UtObject *message, const char *name) {
  UtObjectRef string = ut_string_new_from_utf8(data);
  if (ut_object_implements_error(string)) {
    ut_cstring_ref error_description = ut_error_get_description(string);
    ut_cstring_ref description =
        ut_cstring_new_printf("Invalid string: %s", error_description);
    set_error(self, description);
    return;
  }
  ut_map_insert_string(message, name, string);
}

static void read_len_field(UtProtobufDecoder *self, size_t data_length,
                           size_t *offset, UtObject *message, UtObject *field) {
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

  const char *name = ut_protobuf_message_field_get_name(field);
  UtObject *type = ut_protobuf_message_field_get_type(field);
  if (ut_object_is_protobuf_primitive_type(type)) {
    switch (ut_protobuf_primitive_type_get_type(type)) {
    case UT_PROTOBUF_PRIMITIVE_STRING:
      process_string(self, data, message, name);
      return;
    case UT_PROTOBUF_PRIMITIVE_BYTES:
      ut_map_insert_string(message, name, data);
      return;
    default:
      break;
    }
  }

  set_error(self, "Received LEN for non-LEN field");
}

static void read_i32_field(UtProtobufDecoder *self, size_t data_length,
                           size_t *offset, UtObject *message, UtObject *field) {
  if (field == NULL) {
    read_i32(self, data_length, offset);
    return;
  }

  const char *name = ut_protobuf_message_field_get_name(field);
  UtObject *type = ut_protobuf_message_field_get_type(field);
  if (ut_object_is_protobuf_primitive_type(type)) {
    switch (ut_protobuf_primitive_type_get_type(type)) {
    case UT_PROTOBUF_PRIMITIVE_FIXED32:
      ut_map_insert_string_take(
          message, name,
          ut_uint32_new(read_fixed32(self, data_length, offset)));
      return;
    case UT_PROTOBUF_PRIMITIVE_SFIXED32:
      ut_map_insert_string_take(
          message, name,
          ut_int32_new(read_sfixed32(self, data_length, offset)));
      return;
    case UT_PROTOBUF_PRIMITIVE_FLOAT:
      ut_map_insert_string_take(
          message, name, ut_float32_new(read_float(self, data_length, offset)));
      return;
    default:
      break;
    }
  }

  set_error(self, "Received I32 for non-I32 field");
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
  size_t data_length = ut_list_get_length(self->data);
  size_t offset = 0;
  while (offset < data_length && self->error == NULL) {
    uint64_t tag = read_varint(self, data_length, &offset);

    uint8_t wire_type = tag & 0x7;
    uint32_t number = tag >> 3;
    UtObject *field =
        ut_protobuf_message_type_get_field_by_number(type, number);

    switch (wire_type) {
    case 0:
      read_varint_field(self, data_length, &offset, message, field);
      break;
    case 1:
      read_i64_field(self, data_length, &offset, message, field);
      break;
    case 2:
      read_len_field(self, data_length, &offset, message, field);
      break;
    case 5:
      read_i32_field(self, data_length, &offset, message, field);
      break;
    default:
      set_error(self, "Unknown wire type");
      return ut_map_new();
    }
  }

  // Check for missing fields.
  UtObject *fields = ut_protobuf_message_type_get_fields(type);
  size_t fields_length = ut_list_get_length(fields);
  for (size_t i = 0; i < fields_length; i++) {
    UtObject *field = ut_object_list_get_element(fields, i);
    const char *name = ut_protobuf_message_field_get_name(field);
    if (ut_map_lookup_string(message, name) == NULL) {
      ut_cstring_ref description =
          ut_cstring_new_printf("Missing required field %s", name);
      set_error(self, description);
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
