#include <assert.h>

#include "ut-dbus-message-encoder.h"
#include "ut.h"

typedef struct {
  UtObject object;
} UtDBusMessageEncoder;

static UtObject *header_field_new(uint8_t code, UtObject *value) {
  return ut_dbus_struct_new_take(ut_uint8_new(code), ut_dbus_variant_new(value),
                                 NULL);
}

static UtObject *header_field_new_string(uint8_t code, const char *text) {
  UtObjectRef value = ut_string_new(text);
  return header_field_new(code, value);
}

static void write_padding(UtObject *buffer, size_t count) {
  for (size_t i = 0; i < count; i++) {
    ut_uint8_list_append(buffer, 0);
  }
}

static size_t get_alignment(const char *signature) {
  switch (signature[0]) {
  case 'y': // byte
  case 'g': // signature
  case 'v': // variant
    return 1;
  case 'n': // int16
  case 'q': // uint16
    return 2;
  case 'b': // boolean
  case 'i': // int32
  case 'u': // uint32
  case 's': // string
  case 'o': // object path
  case 'a': // array
  case 'h': // unix fd
    return 4;
  case 'x': // int64
  case 't': // uint64
  case 'd': // double
  case '(': // struct
  case '{': // dict entry
    return 8;
  default:
    return 1;
  }
}

static void write_align_padding(UtObject *buffer, size_t alignment) {
  size_t extra = ut_list_get_length(buffer) % alignment;
  if (extra != 0) {
    write_padding(buffer, alignment - extra);
  }
}

static void rewrite_length(UtObject *buffer, size_t length_offset,
                           uint32_t length) {
  uint8_t *data = ut_uint8_list_get_writable_data(buffer);
  data[length_offset] = length & 0xff;
  data[length_offset + 1] = (length >> 8) & 0xff;
  data[length_offset + 2] = (length >> 16) & 0xff;
  data[length_offset + 3] = length >> 24;
}

static void write_signature(UtObject *buffer, const char *value) {
  size_t value_length = ut_cstring_get_length(value);
  ut_uint8_list_append(buffer, value_length);
  ut_uint8_list_append_block(buffer, (const uint8_t *)value, value_length + 1);
}

static void write_string(UtObject *buffer, const char *value) {
  size_t value_length = ut_cstring_get_length(value);
  write_align_padding(buffer, 4);
  ut_uint8_list_append_uint32_le(buffer, value_length);
  ut_uint8_list_append_block(buffer, (const uint8_t *)value, value_length + 1);
}

static char *get_signature(UtObject *value) {
  if (ut_object_is_uint8(value)) {
    return ut_cstring_new("y");
  } else if (ut_object_is_boolean(value)) {
    return ut_cstring_new("b");
  } else if (ut_object_is_int16(value)) {
    return ut_cstring_new("n");
  } else if (ut_object_is_uint16(value)) {
    return ut_cstring_new("q");
  } else if (ut_object_is_int32(value)) {
    return ut_cstring_new("i");
  } else if (ut_object_is_uint32(value)) {
    return ut_cstring_new("u");
  } else if (ut_object_is_int64(value)) {
    return ut_cstring_new("x");
  } else if (ut_object_is_uint64(value)) {
    return ut_cstring_new("t");
  } else if (ut_object_is_float64(value)) {
    return ut_cstring_new("d");
  } else if (ut_object_is_dbus_object_path(value)) {
    return ut_cstring_new("o");
  } else if (ut_object_is_dbus_signature(value)) {
    return ut_cstring_new("g");
  } else if (ut_object_implements_string(value)) {
    return ut_cstring_new("s");
  } else if (ut_object_is_dbus_array(value)) {
    return ut_cstring_new_printf("a%s",
                                 ut_dbus_array_get_value_signature(value));
  } else if (ut_object_is_dbus_struct(value)) {
    UtObjectRef signature = ut_string_new("(");
    size_t length = ut_list_get_length(value);
    for (size_t i = 0; i < length; i++) {
      ut_cstring_ref value_signature =
          get_signature(ut_object_list_get_element(value, i));
      ut_string_append(signature, value_signature);
    }
    ut_string_append(signature, ")");
    return ut_string_take_text(signature);
  } else if (ut_object_is_dbus_dict(value)) {
    return ut_cstring_new_printf("a{%s%s}",
                                 ut_dbus_dict_get_key_signature(value),
                                 ut_dbus_dict_get_value_signature(value));
  } else if (ut_object_is_dbus_variant(value)) {
    return ut_cstring_new("v");
  } else if (ut_object_is_file_descriptor(value)) {
    return ut_cstring_new("h");
  } else {
    assert(false);
  }
}

static void write_value(UtObject *buffer, UtObject *value) {
  if (ut_object_is_uint8(value)) {
    ut_uint8_list_append(buffer, ut_uint8_get_value(value));
  } else if (ut_object_is_boolean(value)) {
    write_align_padding(buffer, 4);
    ut_uint8_list_append_uint32_le(buffer, ut_boolean_get_value(value) ? 1 : 0);
  } else if (ut_object_is_int16(value)) {
    write_align_padding(buffer, 2);
    ut_uint8_list_append_int16_le(buffer, ut_int16_get_value(value));
  } else if (ut_object_is_uint16(value)) {
    write_align_padding(buffer, 2);
    ut_uint8_list_append_uint16_le(buffer, ut_uint16_get_value(value));
  } else if (ut_object_is_int32(value)) {
    write_align_padding(buffer, 4);
    ut_uint8_list_append_int32_le(buffer, ut_int32_get_value(value));
  } else if (ut_object_is_uint32(value)) {
    write_align_padding(buffer, 4);
    ut_uint8_list_append_uint32_le(buffer, ut_uint32_get_value(value));
  } else if (ut_object_is_int64(value)) {
    write_align_padding(buffer, 8);
    ut_uint8_list_append_int64_le(buffer, ut_int64_get_value(value));
  } else if (ut_object_is_uint64(value)) {
    write_align_padding(buffer, 8);
    ut_uint8_list_append_uint64_le(buffer, ut_uint64_get_value(value));
  } else if (ut_object_is_float64(value)) {
    write_align_padding(buffer, 8);
    ut_uint8_list_append_float64_le(buffer, ut_float64_get_value(value));
  } else if (ut_object_is_dbus_object_path(value)) {
    write_string(buffer, ut_dbus_object_path_get_value(value));
  } else if (ut_object_is_dbus_signature(value)) {
    write_signature(buffer, ut_dbus_signature_get_value(value));
  } else if (ut_object_implements_string(value)) {
    write_string(buffer, ut_string_get_text(value));
  } else if (ut_object_is_dbus_array(value)) {
    write_align_padding(buffer, 4);
    // Length will be overwritten later.
    size_t length_offset = ut_list_get_length(buffer);
    ut_uint8_list_append_uint32_le(buffer, 0);
    write_align_padding(
        buffer, get_alignment(ut_dbus_array_get_value_signature(value)));
    size_t start = ut_list_get_length(buffer);
    size_t length = ut_list_get_length(value);
    for (size_t i = 0; i < length; i++) {
      write_value(buffer, ut_object_list_get_element(value, i));
    }
    rewrite_length(buffer, length_offset, ut_list_get_length(buffer) - start);
  } else if (ut_object_is_dbus_struct(value)) {
    write_align_padding(buffer, 8);
    size_t length = ut_list_get_length(value);
    for (size_t i = 0; i < length; i++) {
      write_value(buffer, ut_object_list_get_element(value, i));
    }
  } else if (ut_object_is_dbus_dict(value)) {
    write_align_padding(buffer, 4);
    // Length will be overwritten later.
    size_t length_offset = ut_list_get_length(buffer);
    ut_uint8_list_append_uint32_le(buffer, 0);
    write_align_padding(buffer, 8);
    size_t start = ut_list_get_length(buffer);
    UtObjectRef items = ut_map_get_items(value);
    size_t length = ut_list_get_length(items);
    for (size_t i = 0; i < length; i++) {
      UtObject *item = ut_object_list_get_element(items, i);
      UtObject *key = ut_map_item_get_key(item);
      UtObject *value = ut_map_item_get_value(item);
      UtObjectRef element = ut_dbus_struct_new(key, value, NULL);
      write_value(buffer, element);
    }
    rewrite_length(buffer, length_offset, ut_list_get_length(buffer) - start);
  } else if (ut_object_is_dbus_variant(value)) {
    UtObject *child_value = ut_dbus_variant_get_value(value);
    ut_cstring_ref signature = get_signature(child_value);
    write_signature(buffer, signature);
    write_value(buffer, child_value);
  } else if (ut_object_is_file_descriptor(value)) {
    assert(false);
  } else {
    assert(false);
  }
}

static UtObjectInterface object_interface = {.type_name =
                                                 "UtDBusMessageEncoder"};

UtObject *ut_dbus_message_encoder_new() {
  return ut_object_new(sizeof(UtDBusMessageEncoder), &object_interface);
}

UtObject *ut_dbus_message_encoder_encode(UtObject *object, UtObject *message) {
  assert(ut_object_is_dbus_message_encoder(object));

  UtObject *args = ut_dbus_message_get_args(message);
  UtObjectRef args_data = ut_uint8_array_new();
  UtObjectRef signature = NULL;
  if (args != NULL) {
    UtObjectRef signature_string = ut_string_new("");
    size_t args_length = ut_list_get_length(args);
    for (size_t i = 0; i < args_length; i++) {
      UtObjectRef arg = ut_list_get_element(args, i);
      write_value(args_data, arg);
      ut_cstring_ref arg_signature = get_signature(arg);
      ut_string_append(signature_string, arg_signature);
    }
    signature = ut_dbus_signature_new(ut_string_get_text(signature_string));
  }

  UtObjectRef data = ut_uint8_array_new();
  ut_uint8_list_append(data, 'l'); // Little endian.
  ut_uint8_list_append(data, ut_dbus_message_get_type(message));
  ut_uint8_list_append(data, ut_dbus_message_get_flags(message));
  ut_uint8_list_append(data, 1); // Protocol version.
  ut_uint8_list_append_uint32_le(data, ut_list_get_length(args_data));
  ut_uint8_list_append_uint32_le(data, ut_dbus_message_get_serial(message));
  UtObjectRef header_fields = ut_dbus_array_new("(yv)");
  const char *path = ut_dbus_message_get_path(message);
  if (path != NULL) {
    UtObjectRef value = ut_dbus_object_path_new(path);
    ut_list_append_take(header_fields, header_field_new(1, value));
  }
  const char *interface = ut_dbus_message_get_interface(message);
  if (interface != NULL) {
    ut_list_append_take(header_fields, header_field_new_string(2, interface));
  }
  const char *member = ut_dbus_message_get_member(message);
  if (member != NULL) {
    ut_list_append_take(header_fields, header_field_new_string(3, member));
  }
  const char *error_name = ut_dbus_message_get_error_name(message);
  if (error_name != NULL) {
    ut_list_append_take(header_fields, header_field_new_string(4, error_name));
  }
  if (ut_dbus_message_has_reply_serial(message)) {
    UtObjectRef value =
        ut_uint32_new(ut_dbus_message_get_reply_serial(message));
    ut_list_append_take(header_fields, header_field_new(5, value));
  }
  const char *destination = ut_dbus_message_get_destination(message);
  if (destination != NULL) {
    ut_list_append_take(header_fields, header_field_new_string(6, destination));
  }
  const char *sender = ut_dbus_message_get_sender(message);
  if (sender != NULL) {
    ut_list_append_take(header_fields, header_field_new_string(7, sender));
  }
  if (signature != NULL) {
    ut_list_append_take(header_fields, header_field_new(8, signature));
  }
  write_value(data, header_fields);

  // Data
  write_align_padding(data, 8);
  ut_list_append_list(data, args_data);

  return ut_object_ref(data);
}

bool ut_object_is_dbus_message_encoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
