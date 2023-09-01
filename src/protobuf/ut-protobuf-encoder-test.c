#include <float.h>

#include "ut.h"

static void test_encode(UtObject *message, UtObject *value,
                        const char *hex_string) {
  UtObjectRef encoder = ut_protobuf_encoder_new();
  ut_protobuf_encoder_encode_message(encoder, message, value);
  ut_assert_null_object(ut_protobuf_encoder_get_error(encoder));
  UtObject *data = ut_protobuf_encoder_get_data(encoder);
  ut_assert_uint8_list_equal_hex(data, hex_string);
}

static void test_uint32(uint32_t value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_take(
              ut_protobuf_primitive_type_new_uint32(), "value", 1),
          NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", ut_uint32_new(value), NULL);
  test_encode(message, message_value, hex_string);
}

static void test_int32(uint32_t value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_take(
              ut_protobuf_primitive_type_new_int32(), "value", 1),
          NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", ut_int32_new(value), NULL);
  test_encode(message, message_value, hex_string);
}

static void test_sint32(int32_t value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_take(
              ut_protobuf_primitive_type_new_sint32(), "value", 1),
          NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", ut_int32_new(value), NULL);
  test_encode(message, message_value, hex_string);
}

static void test_fixed32(uint32_t value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_take(
              ut_protobuf_primitive_type_new_fixed32(), "value", 1),
          NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", ut_uint32_new(value), NULL);
  test_encode(message, message_value, hex_string);
}

static void test_sfixed32(int32_t value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_take(
              ut_protobuf_primitive_type_new_sfixed32(), "value", 1),
          NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", ut_int32_new(value), NULL);
  test_encode(message, message_value, hex_string);
}

static void test_uint64(uint64_t value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_take(
              ut_protobuf_primitive_type_new_uint64(), "value", 1),
          NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", ut_uint64_new(value), NULL);
  test_encode(message, message_value, hex_string);
}

static void test_int64(uint64_t value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_take(
              ut_protobuf_primitive_type_new_int64(), "value", 1),
          NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", ut_int64_new(value), NULL);
  test_encode(message, message_value, hex_string);
}

static void test_sint64(int64_t value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_take(
              ut_protobuf_primitive_type_new_sint64(), "value", 1),
          NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", ut_int64_new(value), NULL);
  test_encode(message, message_value, hex_string);
}

static void test_fixed64(uint64_t value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_take(
              ut_protobuf_primitive_type_new_fixed64(), "value", 1),
          NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", ut_uint64_new(value), NULL);
  test_encode(message, message_value, hex_string);
}

static void test_sfixed64(int64_t value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_take(
              ut_protobuf_primitive_type_new_sfixed64(), "value", 1),
          NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", ut_int64_new(value), NULL);
  test_encode(message, message_value, hex_string);
}

static void test_bool(bool value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_take(
              ut_protobuf_primitive_type_new_bool(), "value", 1),
          NULL));
  UtObjectRef message_value = ut_map_new_string_from_elements_take(
      "value", ut_boolean_new(value), NULL);
  test_encode(message, message_value, hex_string);
}

static void test_float(float value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_take(
              ut_protobuf_primitive_type_new_float(), "value", 1),
          NULL));
  UtObjectRef message_value = ut_map_new_string_from_elements_take(
      "value", ut_float32_new(value), NULL);
  test_encode(message, message_value, hex_string);
}

static void test_double(double value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_take(
              ut_protobuf_primitive_type_new_double(), "value", 1),
          NULL));
  UtObjectRef message_value = ut_map_new_string_from_elements_take(
      "value", ut_float64_new(value), NULL);
  test_encode(message, message_value, hex_string);
}

static void test_string(const char *value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_take(
              ut_protobuf_primitive_type_new_string(), "value", 1),
          NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", ut_string_new(value), NULL);
  test_encode(message, message_value, hex_string);
}

static void test_bytes(const char *hex_value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_take(
              ut_protobuf_primitive_type_new_bytes(), "value", 1),
          NULL));
  UtObjectRef value = ut_uint8_list_new_from_hex_string(hex_value);
  UtObjectRef message_value =
      ut_map_new_string_from_elements("value", value, NULL);
  test_encode(message, message_value, hex_string);
}

static void test_message() {
  UtObjectRef encoder1 = ut_protobuf_encoder_new();
  UtObjectRef message1 =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_take(
              ut_protobuf_primitive_type_new_string(), "name", 1),
          ut_protobuf_message_field_new_take(
              ut_protobuf_primitive_type_new_uint32(), "age", 2),
          NULL));
  UtObjectRef value1 = ut_map_new_string_from_elements_take(
      "name", ut_string_new("Arthur Dent"), "age", ut_uint32_new(42), NULL);
  ut_protobuf_encoder_encode_message(encoder1, message1, value1);
  ut_assert_null_object(ut_protobuf_encoder_get_error(encoder1));
  UtObject *data1 = ut_protobuf_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "0a0b4172746875722044656e74102a");
}

int main(int argc, char **argv) {
  test_uint32(0, "0800");
  test_uint32(1, "0801");
  test_uint32(127, "087f");
  test_uint32(128, "088001");
  test_uint32(UINT32_MAX, "08ffffffff0f");

  test_int32(0, "0800");
  test_int32(1, "0801");
  test_int32(-1, "08ffffffffffffffffff01");
  test_int32(127, "087f");
  test_int32(128, "088001");
  test_int32(INT32_MAX, "08ffffffff07");
  test_int32(INT32_MIN, "0880808080f8ffffffff01");

  test_sint32(0, "0800");
  test_sint32(1, "0802");
  test_sint32(-1, "0801");
  test_sint32(63, "087e");
  test_sint32(64, "088001");
  test_sint32(INT32_MAX, "08feffffff0f");
  test_sint32(INT32_MIN, "08ffffffff0f");

  test_fixed32(0, "0d00000000");
  test_fixed32(1, "0d01000000");
  test_fixed32(0x12345678, "0d78563412");
  test_fixed32(UINT32_MAX, "0dffffffff");

  test_sfixed32(0, "0d00000000");
  test_sfixed32(1, "0d01000000");
  test_sfixed32(-1, "0dffffffff");
  test_sfixed32(0x12345678, "0d78563412");
  test_sfixed32(INT32_MAX, "0dffffff7f");
  test_sfixed32(INT32_MIN, "0d00000080");

  test_uint64(0, "0800");
  test_uint64(1, "0801");
  test_uint64(127, "087f");
  test_uint64(128, "088001");
  test_uint64(UINT64_MAX, "08ffffffffffffffffff01");

  test_int64(0, "0800");
  test_int64(1, "0801");
  test_int64(-1, "08ffffffffffffffffff01");
  test_int64(127, "087f");
  test_int64(128, "088001");
  test_int64(INT64_MAX, "08ffffffffffffffff7f");
  test_int64(INT64_MIN, "0880808080808080808001");

  test_sint64(0, "0800");
  test_sint64(1, "0802");
  test_sint64(-1, "0801");
  test_sint64(63, "087e");
  test_sint64(64, "088001");
  test_sint64(INT64_MAX, "08feffffffffffffffff01");
  test_sint64(INT64_MIN, "08ffffffffffffffffff01");

  test_fixed64(0, "090000000000000000");
  test_fixed64(1, "090100000000000000");
  test_fixed64(0x123456789abcdef0, "09f0debc9a78563412");
  test_fixed64(UINT64_MAX, "09ffffffffffffffff");

  test_sfixed64(0, "090000000000000000");
  test_sfixed64(1, "090100000000000000");
  test_sfixed64(-1, "09ffffffffffffffff");
  test_sfixed64(0x123456789abcdef0, "09f0debc9a78563412");
  test_sfixed64(INT64_MAX, "09ffffffffffffff7f");
  test_sfixed64(INT64_MIN, "090000000000000080");

  test_bool(false, "0800");
  test_bool(true, "0801");

  test_float(0, "0d00000000");
  test_float(1, "0d0000803f");
  test_float(-1, "0d000080bf");
  test_float(3.14159, "0dd00f4940");
  test_float(FLT_MAX, "0dffff7f7f");

  test_double(0, "090000000000000000");
  test_double(1, "09000000000000f03f");
  test_double(-1, "09000000000000f0bf");
  test_double(3.14159, "096e861bf0f9210940");
  test_double(DBL_MAX, "09ffffffffffffef7f");

  test_string("", "0a00");
  test_string("H", "0a0148");
  test_string("Hello World!", "0a0c48656c6c6f20576f726c6421");
  test_string("$¢€𐐷😀", "0a0e24c2a2e282acf09090b7f09f9880");

  test_bytes("", "0a00");
  test_bytes("ff", "0a01ff");
  test_bytes("deadbeef", "0a04deadbeef");

  // test_enum();

  test_message();

  return 0;
}
