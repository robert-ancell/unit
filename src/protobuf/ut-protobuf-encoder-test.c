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

static void test_encode_error(UtObject *message, UtObject *value,
                              const char *error_description) {
  UtObjectRef encoder = ut_protobuf_encoder_new();
  ut_protobuf_encoder_encode_message(encoder, message, value);
  ut_assert_is_error_with_description(ut_protobuf_encoder_get_error(encoder),
                                      error_description);
}

static void test_uint32_value(uint32_t value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_uint32("value", 1), NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", ut_uint32_new(value), NULL);
  test_encode(message, message_value, hex_string);
}

static void test_repeated_uint32_value(UtObject *value,
                                       const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_repeated_uint32("value", 1), NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", value, NULL);
  test_encode(message, message_value, hex_string);
}

static void test_uint32() {
  test_uint32_value(0, "0800");
  test_uint32_value(1, "0801");
  test_uint32_value(127, "087f");
  test_uint32_value(128, "088001");
  test_uint32_value(UINT32_MAX, "08ffffffff0f");

  test_repeated_uint32_value(ut_uint32_list_new(), "");
  test_repeated_uint32_value(ut_uint32_list_new_from_elements(1, 42), "082a");
  test_repeated_uint32_value(ut_uint32_list_new_from_elements(3, 1, 2, 3),
                             "080108020803");
}

static void test_int32_value(uint32_t value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_int32("value", 1), NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", ut_int32_new(value), NULL);
  test_encode(message, message_value, hex_string);
}

static void test_repeated_int32_value(UtObject *value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_repeated_int32("value", 1), NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", value, NULL);
  test_encode(message, message_value, hex_string);
}

static void test_int32() {
  test_int32_value(0, "0800");
  test_int32_value(1, "0801");
  test_int32_value(-1, "08ffffffffffffffffff01");
  test_int32_value(127, "087f");
  test_int32_value(128, "088001");
  test_int32_value(INT32_MAX, "08ffffffff07");
  test_int32_value(INT32_MIN, "0880808080f8ffffffff01");

  test_repeated_int32_value(ut_int32_list_new(), "");
  test_repeated_int32_value(ut_int32_list_new_from_elements(1, 42), "082a");
  test_repeated_int32_value(ut_int32_list_new_from_elements(3, 1, 2, 3),
                            "080108020803");
}

static void test_sint32_value(int32_t value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_sint32("value", 1), NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", ut_int32_new(value), NULL);
  test_encode(message, message_value, hex_string);
}

static void test_repeated_sint32_value(UtObject *value,
                                       const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_repeated_sint32("value", 1), NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", value, NULL);
  test_encode(message, message_value, hex_string);
}

static void test_sint32() {
  test_sint32_value(0, "0800");
  test_sint32_value(1, "0802");
  test_sint32_value(-1, "0801");
  test_sint32_value(63, "087e");
  test_sint32_value(64, "088001");
  test_sint32_value(INT32_MAX, "08feffffff0f");
  test_sint32_value(INT32_MIN, "08ffffffff0f");

  test_repeated_sint32_value(ut_int32_list_new(), "");
  test_repeated_sint32_value(ut_int32_list_new_from_elements(1, 42), "0854");
  test_repeated_sint32_value(ut_int32_list_new_from_elements(3, 1, 2, 3),
                             "080208040806");
}

static void test_fixed32_value(uint32_t value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_fixed32("value", 1), NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", ut_uint32_new(value), NULL);
  test_encode(message, message_value, hex_string);
}

static void test_repeated_fixed32_value(UtObject *value,
                                        const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_repeated_fixed32("value", 1), NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", value, NULL);
  test_encode(message, message_value, hex_string);
}

static void test_fixed32() {
  test_fixed32_value(0, "0d00000000");
  test_fixed32_value(1, "0d01000000");
  test_fixed32_value(0x12345678, "0d78563412");
  test_fixed32_value(UINT32_MAX, "0dffffffff");

  test_repeated_fixed32_value(ut_uint32_list_new(), "");
  test_repeated_fixed32_value(ut_uint32_list_new_from_elements(1, 42),
                              "0d2a000000");
  test_repeated_fixed32_value(ut_uint32_list_new_from_elements(3, 1, 2, 3),
                              "0d010000000d020000000d03000000");
}

static void test_sfixed32_value(int32_t value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_sfixed32("value", 1), NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", ut_int32_new(value), NULL);
  test_encode(message, message_value, hex_string);
}

static void test_repeated_sfixed32_value(UtObject *value,
                                         const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_repeated_sfixed32("value", 1), NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", value, NULL);
  test_encode(message, message_value, hex_string);
}

static void test_sfixed32() {
  test_sfixed32_value(0, "0d00000000");
  test_sfixed32_value(1, "0d01000000");
  test_sfixed32_value(-1, "0dffffffff");
  test_sfixed32_value(0x12345678, "0d78563412");
  test_sfixed32_value(INT32_MAX, "0dffffff7f");
  test_sfixed32_value(INT32_MIN, "0d00000080");

  test_repeated_sfixed32_value(ut_int32_list_new(), "");
  test_repeated_sfixed32_value(ut_int32_list_new_from_elements(1, 42),
                               "0d2a000000");
  test_repeated_sfixed32_value(ut_int32_list_new_from_elements(3, 1, 2, 3),
                               "0d010000000d020000000d03000000");
}

static void test_uint64_value(uint64_t value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_uint64("value", 1), NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", ut_uint64_new(value), NULL);
  test_encode(message, message_value, hex_string);
}

static void test_repeated_uint64_value(UtObject *value,
                                       const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_repeated_uint64("value", 1), NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", value, NULL);
  test_encode(message, message_value, hex_string);
}

static void test_uint64() {
  test_uint64_value(0, "0800");
  test_uint64_value(1, "0801");
  test_uint64_value(127, "087f");
  test_uint64_value(128, "088001");
  test_uint64_value(UINT64_MAX, "08ffffffffffffffffff01");

  test_repeated_uint64_value(ut_uint64_list_new(), "");
  test_repeated_uint64_value(ut_uint64_list_new_from_elements(1, 42), "082a");
  test_repeated_uint64_value(ut_uint64_list_new_from_elements(3, 1, 2, 3),
                             "080108020803");
}

static void test_int64_value(uint64_t value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_int64("value", 1), NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", ut_int64_new(value), NULL);
  test_encode(message, message_value, hex_string);
}

static void test_repeated_int64_value(UtObject *value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_repeated_int64("value", 1), NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", value, NULL);
  test_encode(message, message_value, hex_string);
}

static void test_int64() {
  test_int64_value(0, "0800");
  test_int64_value(1, "0801");
  test_int64_value(-1, "08ffffffffffffffffff01");
  test_int64_value(127, "087f");
  test_int64_value(128, "088001");
  test_int64_value(INT64_MAX, "08ffffffffffffffff7f");
  test_int64_value(INT64_MIN, "0880808080808080808001");

  test_repeated_int64_value(ut_int64_list_new(), "");
  test_repeated_int64_value(ut_int64_list_new_from_elements(1, (int64_t)42),
                            "082a");
  test_repeated_int64_value(
      ut_int64_list_new_from_elements(3, (int64_t)1, (int64_t)2, (int64_t)3),
      "080108020803");
}

static void test_sint64_value(int64_t value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_sint64("value", 1), NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", ut_int64_new(value), NULL);
  test_encode(message, message_value, hex_string);
}

static void test_repeated_sint64_value(UtObject *value,
                                       const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_repeated_sint64("value", 1), NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", value, NULL);
  test_encode(message, message_value, hex_string);
}

static void test_sint64() {
  test_sint64_value(0, "0800");
  test_sint64_value(1, "0802");
  test_sint64_value(-1, "0801");
  test_sint64_value(63, "087e");
  test_sint64_value(64, "088001");
  test_sint64_value(INT64_MAX, "08feffffffffffffffff01");
  test_sint64_value(INT64_MIN, "08ffffffffffffffffff01");

  test_repeated_sint64_value(ut_int64_list_new(), "");
  test_repeated_sint64_value(ut_int64_list_new_from_elements(1, 42), "0854");
  test_repeated_sint64_value(
      ut_int64_list_new_from_elements(3, (int64_t)1, (int64_t)2, (int64_t)3),
      "080208040806");
}

static void test_fixed64_value(uint64_t value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_fixed64("value", 1), NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", ut_uint64_new(value), NULL);
  test_encode(message, message_value, hex_string);
}

static void test_repeated_fixed64_value(UtObject *value,
                                        const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_repeated_fixed64("value", 1), NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", value, NULL);
  test_encode(message, message_value, hex_string);
}

static void test_fixed64() {
  test_fixed64_value(0, "090000000000000000");
  test_fixed64_value(1, "090100000000000000");
  test_fixed64_value(0x123456789abcdef0, "09f0debc9a78563412");
  test_fixed64_value(UINT64_MAX, "09ffffffffffffffff");

  test_repeated_fixed64_value(ut_uint64_list_new(), "");
  test_repeated_fixed64_value(ut_uint64_list_new_from_elements(1, 42),
                              "092a00000000000000");
  test_repeated_fixed64_value(
      ut_uint64_list_new_from_elements(3, (uint64_t)1, (uint64_t)2,
                                       (uint64_t)3),
      "090100000000000000090200000000000000090300000000000000");
}

static void test_sfixed64_value(int64_t value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_sfixed64("value", 1), NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", ut_int64_new(value), NULL);
  test_encode(message, message_value, hex_string);
}

static void test_repeated_sfixed64_value(UtObject *value,
                                         const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_repeated_sfixed64("value", 1), NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", value, NULL);
  test_encode(message, message_value, hex_string);
}

static void test_sfixed64() {
  test_sfixed64_value(0, "090000000000000000");
  test_sfixed64_value(1, "090100000000000000");
  test_sfixed64_value(-1, "09ffffffffffffffff");
  test_sfixed64_value(0x123456789abcdef0, "09f0debc9a78563412");
  test_sfixed64_value(INT64_MAX, "09ffffffffffffff7f");
  test_sfixed64_value(INT64_MIN, "090000000000000080");

  test_repeated_sfixed64_value(ut_int64_list_new(), "");
  test_repeated_sfixed64_value(ut_int64_list_new_from_elements(1, 42),
                               "092a00000000000000");
  test_repeated_sfixed64_value(
      ut_int64_list_new_from_elements(3, (int64_t)1, (int64_t)2, (int64_t)3),
      "090100000000000000090200000000000000090300000000000000");
}

static void test_bool_value(bool value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_bool("value", 1), NULL));
  UtObjectRef message_value = ut_map_new_string_from_elements_take(
      "value", ut_boolean_new(value), NULL);
  test_encode(message, message_value, hex_string);
}

static void test_repeated_bool_value(UtObject *value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_repeated_bool("value", 1), NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements("value", value, NULL);
  test_encode(message, message_value, hex_string);
}

static void test_bool() {
  test_bool_value(false, "0800");
  test_bool_value(true, "0801");

  UtObjectRef v1 = ut_boolean_list_new();
  test_repeated_bool_value(v1, "");
  UtObjectRef v2 = ut_boolean_list_new();
  ut_boolean_list_append(v2, true);
  test_repeated_bool_value(v2, "0801");
  UtObjectRef v3 = ut_boolean_list_new();
  ut_boolean_list_append(v3, false);
  ut_boolean_list_append(v3, true);
  test_repeated_bool_value(v3, "08000801");
}

static void test_float_value(float value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_float("value", 1), NULL));
  UtObjectRef message_value = ut_map_new_string_from_elements_take(
      "value", ut_float32_new(value), NULL);
  test_encode(message, message_value, hex_string);
}

static void test_repeated_float_value(UtObject *value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_repeated_float("value", 1), NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", value, NULL);
  test_encode(message, message_value, hex_string);
}

static void test_float() {
  test_float_value(0, "0d00000000");
  test_float_value(1, "0d0000803f");
  test_float_value(-1, "0d000080bf");
  test_float_value(3.14159, "0dd00f4940");
  test_float_value(FLT_MAX, "0dffff7f7f");

  test_repeated_float_value(ut_float32_list_new(), "");
  test_repeated_float_value(ut_float32_list_new_from_elements(1, 1.0f),
                            "0d0000803f");
  test_repeated_float_value(ut_float32_list_new_from_elements(2, 1.0f, -1.0f),
                            "0d0000803f0d000080bf");
}

static void test_double_value(double value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_double("value", 1), NULL));
  UtObjectRef message_value = ut_map_new_string_from_elements_take(
      "value", ut_float64_new(value), NULL);
  test_encode(message, message_value, hex_string);
}

static void test_repeated_double_value(UtObject *value,
                                       const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_repeated_double("value", 1), NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", value, NULL);
  test_encode(message, message_value, hex_string);
}

static void test_double() {
  test_double_value(0, "090000000000000000");
  test_double_value(1, "09000000000000f03f");
  test_double_value(-1, "09000000000000f0bf");
  test_double_value(3.14159, "096e861bf0f9210940");
  test_double_value(DBL_MAX, "09ffffffffffffef7f");

  test_repeated_double_value(ut_float64_list_new(), "");
  test_repeated_double_value(ut_float64_list_new_from_elements(1, 1.0),
                             "09000000000000f03f");
  test_repeated_double_value(ut_float64_list_new_from_elements(2, 1.0, -1.0),
                             "09000000000000f03f09000000000000f0bf");
}

static void test_string_value(const char *value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_string("value", 1), NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", ut_string_new(value), NULL);
  test_encode(message, message_value, hex_string);
}

static void test_repeated_string_value(UtObject *value,
                                       const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_repeated_string("value", 1), NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", value, NULL);
  test_encode(message, message_value, hex_string);
}

static void test_string() {
  test_string_value("", "0a00");
  test_string_value("H", "0a0148");
  test_string_value("Hello World!", "0a0c48656c6c6f20576f726c6421");
  test_string_value("$¢€𐐷😀", "0a0e24c2a2e282acf09090b7f09f9880");

  test_repeated_string_value(ut_string_list_new(), "");
  test_repeated_string_value(ut_string_list_new_from_elements("H", NULL),
                             "0a0148");
  test_repeated_string_value(ut_string_list_new_from_elements("H", "W", NULL),
                             "0a01480a0157");
}

static void test_bytes_value(const char *hex_value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_bytes("value", 1), NULL));
  UtObjectRef value = ut_uint8_list_new_from_hex_string(hex_value);
  UtObjectRef message_value =
      ut_map_new_string_from_elements("value", value, NULL);
  test_encode(message, message_value, hex_string);
}

static void test_repeated_bytes_value(UtObject *value, const char *hex_string) {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_repeated_bytes("value", 1), NULL));
  UtObjectRef message_value =
      ut_map_new_string_from_elements_take("value", value, NULL);
  test_encode(message, message_value, hex_string);
}

static void test_bytes() {
  test_bytes_value("", "0a00");
  test_bytes_value("ff", "0a01ff");
  test_bytes_value("deadbeef", "0a04deadbeef");

  test_repeated_bytes_value(ut_list_new(), "");
  test_repeated_bytes_value(ut_list_new_from_elements_take(
                                ut_uint8_list_new_from_hex_string("ff"), NULL),
                            "0a01ff");
  test_repeated_bytes_value(ut_list_new_from_elements_take(
                                ut_uint8_list_new_from_hex_string("de"),
                                ut_uint8_list_new_from_hex_string("ad"), NULL),
                            "0a01de0a01ad");
}

static void test_enum() {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_take(
              ut_protobuf_enum_type_new_take(
                  ut_map_new_string_from_elements_take(
                      "ZERO", ut_int32_new(0), "ONE", ut_int32_new(1), "TWO",
                      ut_int32_new(2), NULL)),
              "value", 1),
          NULL));

  UtObjectRef message_value1 =
      ut_map_new_string_from_elements_take("value", ut_string_new("ONE"), NULL);
  test_encode(message, message_value1, "0801");

  UtObjectRef message_value2 =
      ut_map_new_string_from_elements_take("value", ut_string_new("TWO"), NULL);
  test_encode(message, message_value2, "0802");

  UtObjectRef message_value3 = ut_map_new_string_from_elements_take(
      "value", ut_string_new("INVALID"), NULL);
  test_encode_error(message, message_value3, "Unknown enum value INVALID");
}

static void test_message() {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_string("name", 1),
          ut_protobuf_message_field_new_uint32("age", 2), NULL));

  // All values.
  UtObjectRef value1 = ut_map_new_string_from_elements_take(
      "name", ut_string_new("Arthur Dent"), "age", ut_uint32_new(42), NULL);
  test_encode(message, value1, "0a0b4172746875722044656e74102a");

  // Missing name.
  UtObjectRef value2 =
      ut_map_new_string_from_elements_take("age", ut_uint32_new(42), NULL);
  test_encode_error(message, value2, "Missing field name");

  // Missing age.
  UtObjectRef value3 = ut_map_new_string_from_elements_take(
      "name", ut_string_new("Arthur Dent"), NULL);
  test_encode_error(message, value3, "Missing field age");

  // Missing multiple values.
  UtObjectRef value4 = ut_map_new();
  test_encode_error(message, value4, "Missing field name");

  UtObjectRef optional_message =
      ut_protobuf_message_type_new_take(ut_list_new_from_elements_take(
          ut_protobuf_message_field_new_string("name", 1),
          ut_protobuf_message_field_new_optional_uint32("age", 2), NULL));

  // All values.
  UtObjectRef value5 = ut_map_new_string_from_elements_take(
      "name", ut_string_new("Arthur Dent"), "age", ut_uint32_new(42), NULL);
  test_encode(optional_message, value5, "0a0b4172746875722044656e74102a");

  // Missing name.
  UtObjectRef value6 =
      ut_map_new_string_from_elements_take("age", ut_uint32_new(42), NULL);
  test_encode_error(optional_message, value6, "Missing field name");

  // Missing age.
  UtObjectRef value7 = ut_map_new_string_from_elements_take(
      "name", ut_string_new("Arthur Dent"), NULL);
  test_encode(optional_message, value7, "0a0b4172746875722044656e74");

  // Missing multiple values.
  UtObjectRef value8 = ut_map_new();
  test_encode_error(optional_message, value8, "Missing field name");
}

int main(int argc, char **argv) {
  test_uint32();
  test_int32();
  test_sint32();
  test_fixed32();
  test_sfixed32();
  test_uint64();
  test_int64();
  test_sint64();
  test_fixed64();
  test_sfixed64();
  test_bool();
  test_float();
  test_double();
  test_string();
  test_bytes();
  test_enum();
  test_message();

  return 0;
}
