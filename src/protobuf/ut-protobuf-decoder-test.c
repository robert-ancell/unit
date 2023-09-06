#include <float.h>

#include "ut.h"

static UtObject *test_decode(UtObject *message, const char *hex_string) {
  UtObjectRef data = ut_uint8_list_new_from_hex_string(hex_string);
  UtObjectRef decoder = ut_protobuf_decoder_new(data);
  UtObjectRef value = ut_protobuf_decoder_decode_message(decoder, message);
  ut_assert_null_object(ut_protobuf_decoder_get_error(decoder));
  return ut_object_ref(value);
}

static void test_decode_error(UtObject *message, const char *hex_string,
                              const char *error_description) {
  UtObjectRef data = ut_uint8_list_new_from_hex_string(hex_string);
  UtObjectRef decoder = ut_protobuf_decoder_new(data);
  UtObjectRef value = ut_protobuf_decoder_decode_message(decoder, message);
  ut_assert_is_error_with_description(ut_protobuf_decoder_get_error(decoder),
                                      error_description);
}

static void test_uint32_value(UtObject *message, const char *hex_string,
                              uint32_t value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_is_uint32(v));
  ut_assert_int_equal(ut_uint32_get_value(v), value);
}

static void test_repeated_uint32_value(UtObject *message,
                                       const char *hex_string,
                                       UtObject *value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_implements_uint32_list(v));
  ut_assert_equal(v, value);
  ut_object_unref(value);
}

static void test_uint32() {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_uint32(1), NULL));

  test_uint32_value(message, "0800", 0);
  test_uint32_value(message, "0801", 1);
  test_uint32_value(message, "087f", 127);
  test_uint32_value(message, "088001", 128);
  test_uint32_value(message, "08ffffffff0f", UINT32_MAX);

  // Second value is the one kept.
  test_uint32_value(message, "08010802", 2);

  test_decode_error(message, "", "Missing required field value");
  test_decode_error(message, "090000000000000000",
                    "Received I64 for non-I64 field");
  test_decode_error(message, "0a00", "Received LEN for non-LEN field");
  test_decode_error(message, "0b00", "Unknown wire type");
  test_decode_error(message, "0c00", "Unknown wire type");
  test_decode_error(message, "0d00000000", "Received I32 for non-I32 field");
  test_decode_error(message, "0e00", "Unknown wire type");
  test_decode_error(message, "0f00", "Unknown wire type");

  test_decode_error(message, "088080808010",
                    "VARINT too large for 32 bit value");

  UtObjectRef optional_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_optional_uint32(1), NULL));
  test_uint32_value(optional_message, "", 0);
  test_uint32_value(optional_message, "0801", 1);
  test_uint32_value(optional_message, "08010802", 2);

  UtObjectRef repeated_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_repeated_uint32(1), NULL));
  test_repeated_uint32_value(repeated_message, "", ut_uint32_list_new());
  test_repeated_uint32_value(repeated_message, "0801",
                             ut_uint32_list_new_from_elements(1, 1));
  test_repeated_uint32_value(repeated_message, "08010802",
                             ut_uint32_list_new_from_elements(2, 1, 2));
}

static void test_int32_value(UtObject *message, const char *hex_string,
                             int32_t value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_is_int32(v));
  ut_assert_int_equal(ut_int32_get_value(v), value);
}

static void test_repeated_int32_value(UtObject *message, const char *hex_string,
                                      UtObject *value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_implements_int32_list(v));
  ut_assert_equal(v, value);
  ut_object_unref(value);
}

static void test_int32() {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_int32(1), NULL));

  test_int32_value(message, "0800", 0);
  test_int32_value(message, "0801", 1);
  test_int32_value(message, "08ffffffffffffffffff01", -1);
  test_int32_value(message, "087f", 127);
  test_int32_value(message, "088001", 128);
  test_int32_value(message, "08ffffffff07", INT32_MAX);
  test_int32_value(message, "0880808080f8ffffffff01", INT32_MIN);

  // Second value is the one kept.
  test_int32_value(message, "08010802", 2);

  test_decode_error(message, "", "Missing required field value");
  test_decode_error(message, "090000000000000000",
                    "Received I64 for non-I64 field");
  test_decode_error(message, "0a00", "Received LEN for non-LEN field");
  test_decode_error(message, "0b00", "Unknown wire type");
  test_decode_error(message, "0c00", "Unknown wire type");
  test_decode_error(message, "0d00000000", "Received I32 for non-I32 field");
  test_decode_error(message, "0e00", "Unknown wire type");
  test_decode_error(message, "0f00", "Unknown wire type");

  test_decode_error(message, "088080808010",
                    "VARINT too large for signed 32 bit value");
  test_decode_error(message, "0880808080808080808001",
                    "VARINT too large for signed 32 bit value");

  UtObjectRef optional_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_optional_int32(1), NULL));
  test_int32_value(optional_message, "", 0);
  test_int32_value(optional_message, "0801", 1);
  test_int32_value(optional_message, "08010802", 2);

  UtObjectRef repeated_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_repeated_int32(1), NULL));
  test_repeated_int32_value(repeated_message, "", ut_int32_list_new());
  test_repeated_int32_value(repeated_message, "0801",
                            ut_int32_list_new_from_elements(1, 1));
  test_repeated_int32_value(repeated_message, "08010802",
                            ut_int32_list_new_from_elements(2, 1, 2));
}

static void test_sint32_value(UtObject *message, const char *hex_string,
                              int32_t value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_is_int32(v));
  ut_assert_int_equal(ut_int32_get_value(v), value);
}

static void test_repeated_sint32_value(UtObject *message,
                                       const char *hex_string,
                                       UtObject *value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_implements_int32_list(v));
  ut_assert_equal(v, value);
  ut_object_unref(value);
}

static void test_sint32() {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_sint32(1), NULL));

  test_sint32_value(message, "0800", 0);
  test_sint32_value(message, "0802", 1);
  test_sint32_value(message, "0801", -1);
  test_sint32_value(message, "087e", 63);
  test_sint32_value(message, "088001", 64);
  test_sint32_value(message, "08feffffff0f", INT32_MAX);
  test_sint32_value(message, "08ffffffff0f", INT32_MIN);

  // Second value is the one kept.
  test_sint32_value(message, "08020804", 2);

  test_decode_error(message, "", "Missing required field value");
  test_decode_error(message, "090000000000000000",
                    "Received I64 for non-I64 field");
  test_decode_error(message, "0a00", "Received LEN for non-LEN field");
  test_decode_error(message, "0b00", "Unknown wire type");
  test_decode_error(message, "0c00", "Unknown wire type");
  test_decode_error(message, "0d00000000", "Received I32 for non-I32 field");
  test_decode_error(message, "0e00", "Unknown wire type");
  test_decode_error(message, "0f00", "Unknown wire type");

  test_decode_error(message, "088080808010",
                    "VARINT too large for 32 bit value");

  UtObjectRef optional_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_optional_sint32(1), NULL));
  test_sint32_value(optional_message, "", 0);
  test_sint32_value(optional_message, "0802", 1);
  test_sint32_value(optional_message, "08020804", 2);

  UtObjectRef repeated_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_repeated_sint32(1), NULL));
  test_repeated_sint32_value(repeated_message, "", ut_int32_list_new());
  test_repeated_sint32_value(repeated_message, "0802",
                             ut_int32_list_new_from_elements(1, 1));
  test_repeated_sint32_value(repeated_message, "08020804",
                             ut_int32_list_new_from_elements(2, 1, 2));
}

static void test_fixed32_value(UtObject *message, const char *hex_string,
                               uint32_t value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_is_uint32(v));
  ut_assert_int_equal(ut_uint32_get_value(v), value);
}

static void test_repeated_fixed32_value(UtObject *message,
                                        const char *hex_string,
                                        UtObject *value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_implements_uint32_list(v));
  ut_assert_equal(v, value);
  ut_object_unref(value);
}

static void test_fixed32() {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_fixed32(1), NULL));

  test_fixed32_value(message, "0d00000000", 0);
  test_fixed32_value(message, "0d01000000", 1);
  test_fixed32_value(message, "0d78563412", 0x12345678);
  test_fixed32_value(message, "0dffffffff", UINT32_MAX);

  // Second value is the one kept.
  test_fixed32_value(message, "0d010000000d02000000", 2);

  test_decode_error(message, "", "Missing required field value");
  test_decode_error(message, "0800", "Received VARINT for non-VARINT field");
  test_decode_error(message, "090000000000000000",
                    "Received I64 for non-I64 field");
  test_decode_error(message, "0a00", "Received LEN for non-LEN field");
  test_decode_error(message, "0b00", "Unknown wire type");
  test_decode_error(message, "0c00", "Unknown wire type");
  test_decode_error(message, "0e00", "Unknown wire type");
  test_decode_error(message, "0f00", "Unknown wire type");

  test_decode_error(message, "0d", "Insufficient data");
  test_decode_error(message, "0d00", "Insufficient data");
  test_decode_error(message, "0d0000", "Insufficient data");
  test_decode_error(message, "0d000000", "Insufficient data");

  UtObjectRef optional_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_optional_fixed32(1), NULL));
  test_fixed32_value(optional_message, "", 0);
  test_fixed32_value(optional_message, "0d01000000", 1);
  test_fixed32_value(optional_message, "0d010000000d02000000", 2);

  UtObjectRef repeated_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_repeated_fixed32(1), NULL));
  test_repeated_fixed32_value(repeated_message, "", ut_uint32_list_new());
  test_repeated_fixed32_value(repeated_message, "0d01000000",
                              ut_uint32_list_new_from_elements(1, 1));
  test_repeated_fixed32_value(repeated_message, "0d010000000d02000000",
                              ut_uint32_list_new_from_elements(2, 1, 2));
}

static void test_sfixed32_value(UtObject *message, const char *hex_string,
                                int32_t value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_is_int32(v));
  ut_assert_int_equal(ut_int32_get_value(v), value);
}

static void test_repeated_sfixed32_value(UtObject *message,
                                         const char *hex_string,
                                         UtObject *value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_implements_int32_list(v));
  ut_assert_equal(v, value);
  ut_object_unref(value);
}

static void test_sfixed32() {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_sfixed32(1), NULL));

  test_sfixed32_value(message, "0d00000000", 0);
  test_sfixed32_value(message, "0d01000000", 1);
  test_sfixed32_value(message, "0dffffffff", -1);
  test_sfixed32_value(message, "0d78563412", 0x12345678);
  test_sfixed32_value(message, "0dffffff7f", INT32_MAX);
  test_sfixed32_value(message, "0d00000080", INT32_MIN);

  // Second value is the one kept.
  test_sfixed32_value(message, "0d010000000d02000000", 2);

  test_decode_error(message, "", "Missing required field value");
  test_decode_error(message, "0800", "Received VARINT for non-VARINT field");
  test_decode_error(message, "090000000000000000",
                    "Received I64 for non-I64 field");
  test_decode_error(message, "0a00", "Received LEN for non-LEN field");
  test_decode_error(message, "0b00", "Unknown wire type");
  test_decode_error(message, "0c00", "Unknown wire type");
  test_decode_error(message, "0e00", "Unknown wire type");
  test_decode_error(message, "0f00", "Unknown wire type");

  test_decode_error(message, "0d", "Insufficient data");
  test_decode_error(message, "0d00", "Insufficient data");
  test_decode_error(message, "0d0000", "Insufficient data");
  test_decode_error(message, "0d000000", "Insufficient data");

  UtObjectRef optional_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_optional_sfixed32(1), NULL));
  test_sfixed32_value(optional_message, "", 0);
  test_sfixed32_value(optional_message, "0d01000000", 1);
  test_sfixed32_value(optional_message, "0d010000000d02000000", 2);

  UtObjectRef repeated_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_repeated_sfixed32(1), NULL));
  test_repeated_sfixed32_value(repeated_message, "", ut_int32_list_new());
  test_repeated_sfixed32_value(repeated_message, "0d01000000",
                               ut_int32_list_new_from_elements(1, 1));
  test_repeated_sfixed32_value(repeated_message, "0d010000000d02000000",
                               ut_int32_list_new_from_elements(2, 1, 2));
}

static void test_uint64_value(UtObject *message, const char *hex_string,
                              uint64_t value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_is_uint64(v));
  ut_assert_int_equal(ut_uint64_get_value(v), value);
}

static void test_repeated_uint64_value(UtObject *message,
                                       const char *hex_string,
                                       UtObject *value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_implements_uint64_list(v));
  ut_assert_equal(v, value);
  ut_object_unref(value);
}

static void test_uint64() {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_uint64(1), NULL));

  test_uint64_value(message, "0800", 0);
  test_uint64_value(message, "0801", 1);
  test_uint64_value(message, "087f", 127);
  test_uint64_value(message, "088001", 128);
  test_uint64_value(message, "08ffffffffffffffffff01", UINT64_MAX);

  // Second value is the one kept.
  test_uint64_value(message, "08010802", 2);

  test_decode_error(message, "", "Missing required field value");
  test_decode_error(message, "090000000000000000",
                    "Received I64 for non-I64 field");
  test_decode_error(message, "0a00", "Received LEN for non-LEN field");
  test_decode_error(message, "0b00", "Unknown wire type");
  test_decode_error(message, "0c00", "Unknown wire type");
  test_decode_error(message, "0d00000000", "Received I32 for non-I32 field");
  test_decode_error(message, "0e00", "Unknown wire type");
  test_decode_error(message, "0f00", "Unknown wire type");

  test_decode_error(message, "0880808080808080808002",
                    "VARINT greater than 64 bit not supported");

  UtObjectRef optional_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_optional_uint64(1), NULL));
  test_uint64_value(optional_message, "", 0);
  test_uint64_value(optional_message, "0801", 1);
  test_uint64_value(optional_message, "08010802", 2);

  UtObjectRef repeated_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_repeated_uint64(1), NULL));
  test_repeated_uint64_value(repeated_message, "", ut_uint64_list_new());
  test_repeated_uint64_value(repeated_message, "0801",
                             ut_uint64_list_new_from_elements(1, 1));
  test_repeated_uint64_value(repeated_message, "08010802",
                             ut_uint64_list_new_from_elements(2, 1, 2));
}

static void test_int64_value(UtObject *message, const char *hex_string,
                             int64_t value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_is_int64(v));
  ut_assert_int_equal(ut_int64_get_value(v), value);
}

static void test_repeated_int64_value(UtObject *message, const char *hex_string,
                                      UtObject *value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_implements_int64_list(v));
  ut_assert_equal(v, value);
  ut_object_unref(value);
}

static void test_int64() {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_int64(1), NULL));

  test_int64_value(message, "0800", 0);
  test_int64_value(message, "0801", 1);
  test_int64_value(message, "08ffffffffffffffffff01", -1);
  test_int64_value(message, "087f", 127);
  test_int64_value(message, "088001", 128);
  test_int64_value(message, "08ffffffffffffffff7f", INT64_MAX);
  test_int64_value(message, "0880808080808080808001", INT64_MIN);

  // Second value is the one kept.
  test_int64_value(message, "08010802", 2);

  test_decode_error(message, "", "Missing required field value");
  test_decode_error(message, "090000000000000000",
                    "Received I64 for non-I64 field");
  test_decode_error(message, "0a00", "Received LEN for non-LEN field");
  test_decode_error(message, "0b00", "Unknown wire type");
  test_decode_error(message, "0c00", "Unknown wire type");
  test_decode_error(message, "0d00000000", "Received I32 for non-I32 field");
  test_decode_error(message, "0e00", "Unknown wire type");
  test_decode_error(message, "0f00", "Unknown wire type");

  test_decode_error(message, "0880808080808080808002",
                    "VARINT greater than 64 bit not supported");

  UtObjectRef optional_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_optional_int64(1), NULL));
  test_int64_value(optional_message, "", 0);
  test_int64_value(optional_message, "0801", 1);
  test_int64_value(optional_message, "08010802", 2);

  UtObjectRef repeated_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_repeated_int64(1), NULL));
  test_repeated_int64_value(repeated_message, "", ut_int64_list_new());
  test_repeated_int64_value(repeated_message, "0801",
                            ut_int64_list_new_from_elements(1, 1));
  test_repeated_int64_value(repeated_message, "08010802",
                            ut_int64_list_new_from_elements(2, 1, 2));
}

static void test_sint64_value(UtObject *message, const char *hex_string,
                              int64_t value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_is_int64(v));
  ut_assert_int_equal(ut_int64_get_value(v), value);
}

static void test_repeated_sint64_value(UtObject *message,
                                       const char *hex_string,
                                       UtObject *value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_implements_int64_list(v));
  ut_assert_equal(v, value);
  ut_object_unref(value);
}

static void test_sint64() {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_sint64(1), NULL));

  test_sint64_value(message, "0800", 0);
  test_sint64_value(message, "0802", 1);
  test_sint64_value(message, "0801", -1);
  test_sint64_value(message, "087e", 63);
  test_sint64_value(message, "088001", 64);
  test_sint64_value(message, "08feffffffffffffffff01", INT64_MAX);
  test_sint64_value(message, "08ffffffffffffffffff01", INT64_MIN);

  // Second value is the one kept.
  test_sint64_value(message, "08020804", 2);

  test_decode_error(message, "", "Missing required field value");
  test_decode_error(message, "090000000000000000",
                    "Received I64 for non-I64 field");
  test_decode_error(message, "0a00", "Received LEN for non-LEN field");
  test_decode_error(message, "0b00", "Unknown wire type");
  test_decode_error(message, "0c00", "Unknown wire type");
  test_decode_error(message, "0d00000000", "Received I32 for non-I32 field");
  test_decode_error(message, "0e00", "Unknown wire type");
  test_decode_error(message, "0f00", "Unknown wire type");

  test_decode_error(message, "0880808080808080808002",
                    "VARINT greater than 64 bit not supported");

  UtObjectRef optional_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_optional_sint64(1), NULL));
  test_sint64_value(optional_message, "", 0);
  test_sint64_value(optional_message, "0802", 1);
  test_sint64_value(optional_message, "08020804", 2);

  UtObjectRef repeated_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_repeated_sint64(1), NULL));
  test_repeated_sint64_value(repeated_message, "", ut_int64_list_new());
  test_repeated_sint64_value(repeated_message, "0802",
                             ut_int64_list_new_from_elements(1, 1));
  test_repeated_sint64_value(repeated_message, "08020804",
                             ut_int64_list_new_from_elements(2, 1, 2));
}

static void test_fixed64_value(UtObject *message, const char *hex_string,
                               uint64_t value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_is_uint64(v));
  ut_assert_int_equal(ut_uint64_get_value(v), value);
}

static void test_repeated_fixed64_value(UtObject *message,
                                        const char *hex_string,
                                        UtObject *value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_implements_uint64_list(v));
  ut_assert_equal(v, value);
  ut_object_unref(value);
}

static void test_fixed64() {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_fixed64(1), NULL));

  test_fixed64_value(message, "090000000000000000", 0);
  test_fixed64_value(message, "090100000000000000", 1);
  test_fixed64_value(message, "09f0debc9a78563412", 0x123456789abcdef0);
  test_fixed64_value(message, "09ffffffffffffffff", UINT64_MAX);

  // Second value is the one kept.
  test_fixed64_value(message, "090100000000000000090200000000000000", 2);

  test_decode_error(message, "", "Missing required field value");
  test_decode_error(message, "0800", "Received VARINT for non-VARINT field");
  test_decode_error(message, "0a00", "Received LEN for non-LEN field");
  test_decode_error(message, "0b00", "Unknown wire type");
  test_decode_error(message, "0c00", "Unknown wire type");
  test_decode_error(message, "0d00000000", "Received I32 for non-I32 field");
  test_decode_error(message, "0e00", "Unknown wire type");
  test_decode_error(message, "0f00", "Unknown wire type");

  test_decode_error(message, "09", "Insufficient data");
  test_decode_error(message, "0900", "Insufficient data");
  test_decode_error(message, "090000", "Insufficient data");
  test_decode_error(message, "09000000", "Insufficient data");
  test_decode_error(message, "0900000000", "Insufficient data");
  test_decode_error(message, "090000000000", "Insufficient data");
  test_decode_error(message, "09000000000000", "Insufficient data");
  test_decode_error(message, "0900000000000000", "Insufficient data");

  UtObjectRef optional_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_optional_fixed64(1), NULL));
  test_fixed64_value(optional_message, "", 0);
  test_fixed64_value(optional_message, "090100000000000000", 1);
  test_fixed64_value(optional_message, "090100000000000000090200000000000000",
                     2);

  UtObjectRef repeated_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_repeated_fixed64(1), NULL));
  test_repeated_fixed64_value(repeated_message, "", ut_uint64_list_new());
  test_repeated_fixed64_value(repeated_message, "090100000000000000",
                              ut_uint64_list_new_from_elements(1, 1));
  test_repeated_fixed64_value(repeated_message,
                              "090100000000000000090200000000000000",
                              ut_uint64_list_new_from_elements(2, 1, 2));
}

static void test_sfixed64_value(UtObject *message, const char *hex_string,
                                int64_t value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_is_int64(v));
  ut_assert_int_equal(ut_int64_get_value(v), value);
}

static void test_repeated_sfixed64_value(UtObject *message,
                                         const char *hex_string,
                                         UtObject *value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_implements_int64_list(v));
  ut_assert_equal(v, value);
  ut_object_unref(value);
}

static void test_sfixed64() {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_sfixed64(1), NULL));

  test_sfixed64_value(message, "090000000000000000", 0);
  test_sfixed64_value(message, "090100000000000000", 1);
  test_sfixed64_value(message, "09ffffffffffffffff", -1);
  test_sfixed64_value(message, "09f0debc9a78563412", 0x123456789abcdef0);
  test_sfixed64_value(message, "09ffffffffffffff7f", INT64_MAX);
  test_sfixed64_value(message, "090000000000000080", INT64_MIN);

  // Second value is the one kept.
  test_sfixed64_value(message, "09010000000000000009ffffffffffffffff", -1);

  test_decode_error(message, "", "Missing required field value");
  test_decode_error(message, "0800", "Received VARINT for non-VARINT field");
  test_decode_error(message, "0a00", "Received LEN for non-LEN field");
  test_decode_error(message, "0b00", "Unknown wire type");
  test_decode_error(message, "0c00", "Unknown wire type");
  test_decode_error(message, "0d00000000", "Received I32 for non-I32 field");
  test_decode_error(message, "0e00", "Unknown wire type");
  test_decode_error(message, "0f00", "Unknown wire type");

  test_decode_error(message, "09", "Insufficient data");
  test_decode_error(message, "0900", "Insufficient data");
  test_decode_error(message, "090000", "Insufficient data");
  test_decode_error(message, "09000000", "Insufficient data");
  test_decode_error(message, "0900000000", "Insufficient data");
  test_decode_error(message, "090000000000", "Insufficient data");
  test_decode_error(message, "09000000000000", "Insufficient data");
  test_decode_error(message, "0900000000000000", "Insufficient data");

  UtObjectRef optional_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_optional_sfixed64(1), NULL));
  test_sfixed64_value(optional_message, "", 0);
  test_sfixed64_value(optional_message, "090100000000000000", 1);
  test_sfixed64_value(optional_message, "09010000000000000009ffffffffffffffff",
                      -1);

  UtObjectRef repeated_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_repeated_sfixed64(1), NULL));
  test_repeated_sfixed64_value(repeated_message, "", ut_int64_list_new());
  test_repeated_sfixed64_value(repeated_message, "090100000000000000",
                               ut_int64_list_new_from_elements(1, 1));
  test_repeated_sfixed64_value(
      repeated_message, "09010000000000000009ffffffffffffffff",
      ut_int64_list_new_from_elements(2, (int64_t)1, (int64_t)-1));
}

static void test_bool_value(UtObject *message, const char *hex_string,
                            bool value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_is_boolean(v));
  if (value) {
    ut_assert_true(ut_boolean_get_value(v));
  } else {
    ut_assert_false(ut_boolean_get_value(v));
  }
}

static void test_repeated_bool_value(UtObject *message, const char *hex_string,
                                     UtObject *value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_implements_boolean_list(v));
  ut_assert_equal(v, value);
}

static void test_bool() {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_bool(1), NULL));

  test_bool_value(message, "0800", false);
  test_bool_value(message, "0801", true);

  // Second value is the one kept.
  test_bool_value(message, "08000801", true);

  test_decode_error(message, "", "Missing required field value");
  test_decode_error(message, "090000000000000000",
                    "Received I64 for non-I64 field");
  test_decode_error(message, "0a00", "Received LEN for non-LEN field");
  test_decode_error(message, "0b00", "Unknown wire type");
  test_decode_error(message, "0c00", "Unknown wire type");
  test_decode_error(message, "0d00000000", "Received I32 for non-I32 field");
  test_decode_error(message, "0e00", "Unknown wire type");
  test_decode_error(message, "0f00", "Unknown wire type");

  test_decode_error(message, "0802", "Invalid bool value");
  test_decode_error(message, "087f", "Invalid bool value");
  test_decode_error(message, "08ff7f", "Invalid bool value");

  UtObjectRef optional_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_optional_bool(1), NULL));
  test_bool_value(optional_message, "", false);
  test_bool_value(optional_message, "0801", true);
  test_bool_value(optional_message, "08000801", true);

  UtObjectRef repeated_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_repeated_bool(1), NULL));
  UtObjectRef v1 = ut_boolean_list_new();
  test_repeated_bool_value(repeated_message, "", v1);
  UtObjectRef v2 = ut_boolean_list_new();
  ut_boolean_list_append(v2, true);
  test_repeated_bool_value(repeated_message, "0801", v2);
  UtObjectRef v3 = ut_boolean_list_new();
  ut_boolean_list_append(v3, false);
  ut_boolean_list_append(v3, true);
  test_repeated_bool_value(repeated_message, "08000801", v3);
}

static void test_float_value(UtObject *message, const char *hex_string,
                             float value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_is_float32(v));
  ut_assert_int_equal(ut_float32_get_value(v), value);
}

static void test_repeated_float_value(UtObject *message, const char *hex_string,
                                      UtObject *value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_implements_float32_list(v));
  ut_assert_equal(v, value);
  ut_object_unref(value);
}

static void test_float() {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_float(1), NULL));

  test_float_value(message, "0d00000000", 0);
  test_float_value(message, "0d0000803f", 1);
  test_float_value(message, "0d000080bf", -1);
  test_float_value(message, "0dd00f4940", 3.14159);
  test_float_value(message, "0dffff7f7f", FLT_MAX);

  // Second value is the one kept.
  test_float_value(message, "0d0000803f0d000080bf", -1);

  test_decode_error(message, "", "Missing required field value");
  test_decode_error(message, "0800", "Received VARINT for non-VARINT field");
  test_decode_error(message, "090000000000000000",
                    "Received I64 for non-I64 field");
  test_decode_error(message, "0a00", "Received LEN for non-LEN field");
  test_decode_error(message, "0b00", "Unknown wire type");
  test_decode_error(message, "0c00", "Unknown wire type");
  test_decode_error(message, "0e00", "Unknown wire type");
  test_decode_error(message, "0f00", "Unknown wire type");

  test_decode_error(message, "0d", "Insufficient data");
  test_decode_error(message, "0d00", "Insufficient data");
  test_decode_error(message, "0d0000", "Insufficient data");
  test_decode_error(message, "0d000000", "Insufficient data");

  UtObjectRef optional_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_optional_float(1), NULL));
  test_float_value(optional_message, "", 0);
  test_float_value(optional_message, "0d0000803f", 1);
  test_float_value(optional_message, "0d0000803f0d000080bf", -1);

  UtObjectRef repeated_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_repeated_float(1), NULL));
  test_repeated_float_value(repeated_message, "", ut_float32_list_new());
  test_repeated_float_value(repeated_message, "0d0000803f",
                            ut_float32_list_new_from_elements(1, 1.0f));
  test_repeated_float_value(repeated_message, "0d0000803f0d000080bf",
                            ut_float32_list_new_from_elements(2, 1.0f, -1.0f));
}

static void test_double_value(UtObject *message, const char *hex_string,
                              double value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_is_float64(v));
  ut_assert_int_equal(ut_float64_get_value(v), value);
}

static void test_repeated_double_value(UtObject *message,
                                       const char *hex_string,
                                       UtObject *value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_implements_float64_list(v));
  ut_assert_equal(v, value);
  ut_object_unref(value);
}

static void test_double() {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_double(1), NULL));

  test_double_value(message, "090000000000000000", 0);
  test_double_value(message, "09000000000000f03f", 1);
  test_double_value(message, "09000000000000f0bf", -1);
  test_double_value(message, "096e861bf0f9210940", 3.14159);
  test_double_value(message, "09ffffffffffffef7f", DBL_MAX);

  // Second value is the one kept.
  test_double_value(message, "09000000000000f03f09000000000000f0bf", -1);

  test_decode_error(message, "", "Missing required field value");
  test_decode_error(message, "0800", "Received VARINT for non-VARINT field");
  test_decode_error(message, "0a00", "Received LEN for non-LEN field");
  test_decode_error(message, "0b00", "Unknown wire type");
  test_decode_error(message, "0c00", "Unknown wire type");
  test_decode_error(message, "0d00000000", "Received I32 for non-I32 field");
  test_decode_error(message, "0e00", "Unknown wire type");
  test_decode_error(message, "0f00", "Unknown wire type");

  test_decode_error(message, "09", "Insufficient data");
  test_decode_error(message, "0900", "Insufficient data");
  test_decode_error(message, "090000", "Insufficient data");
  test_decode_error(message, "09000000", "Insufficient data");
  test_decode_error(message, "0900000000", "Insufficient data");
  test_decode_error(message, "090000000000", "Insufficient data");
  test_decode_error(message, "09000000000000", "Insufficient data");
  test_decode_error(message, "0900000000000000", "Insufficient data");

  UtObjectRef optional_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_optional_double(1), NULL));
  test_double_value(optional_message, "", 0);
  test_double_value(optional_message, "09000000000000f03f", 1);
  test_double_value(optional_message, "09000000000000f03f09000000000000f0bf",
                    -1);

  UtObjectRef repeated_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_repeated_double(1), NULL));
  test_repeated_double_value(repeated_message, "", ut_float64_list_new());
  test_repeated_double_value(repeated_message, "09000000000000f03f",
                             ut_float64_list_new_from_elements(1, 1.0));
  test_repeated_double_value(repeated_message,
                             "09000000000000f03f09000000000000f0bf",
                             ut_float64_list_new_from_elements(2, 1.0, -1.0));
}

static void test_string_value(UtObject *message, const char *hex_string,
                              const char *value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_implements_string(v));
  ut_assert_cstring_equal(ut_string_get_text(v), value);
}

static void test_repeated_string_value(UtObject *message,
                                       const char *hex_string,
                                       UtObject *value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_implements_string_list(v));
  ut_assert_equal(v, value);
  ut_object_unref(value);
}

static void test_string() {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_string(1), NULL));

  test_string_value(message, "0a00", "");
  test_string_value(message, "0a0148", "H");
  test_string_value(message, "0a0c48656c6c6f20576f726c6421", "Hello World!");
  test_string_value(message, "0a0e24c2a2e282acf09090b7f09f9880", "$¢€𐐷😀");

  // Second value is the one kept.
  test_string_value(message, "0a01480a0157", "W");

  // FIXME: Invalid UTF-8.

  test_decode_error(message, "", "Missing required field value");
  test_decode_error(message, "0800", "Received VARINT for non-VARINT field");
  test_decode_error(message, "090000000000000000",
                    "Received I64 for non-I64 field");
  test_decode_error(message, "0b00", "Unknown wire type");
  test_decode_error(message, "0c00", "Unknown wire type");
  test_decode_error(message, "0d00000000", "Received I32 for non-I32 field");
  test_decode_error(message, "0e00", "Unknown wire type");
  test_decode_error(message, "0f00", "Unknown wire type");

  test_decode_error(message, "0a03", "Insufficient space for LEN data");
  test_decode_error(message, "0a03ffff", "Insufficient space for LEN data");

  UtObjectRef optional_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_optional_string(1), NULL));
  test_string_value(optional_message, "", "");
  test_string_value(optional_message, "0a0148", "H");
  test_string_value(optional_message, "0a01480a0157", "W");

  UtObjectRef repeated_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_repeated_string(1), NULL));
  test_repeated_string_value(repeated_message, "", ut_string_list_new());
  test_repeated_string_value(repeated_message, "0a0148",
                             ut_string_list_new_from_elements("H", NULL));
  test_repeated_string_value(repeated_message, "0a01480a0157",
                             ut_string_list_new_from_elements("H", "W", NULL));
}

static void test_bytes_value(UtObject *message, const char *hex_string,
                             const char *hex_value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_implements_uint8_list(v));
  ut_assert_uint8_list_equal_hex(v, hex_value);
}

static void test_repeated_bytes_value(UtObject *message, const char *hex_string,
                                      UtObject *value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_implements_object_list(v));
  ut_assert_equal(v, value);
  ut_object_unref(value);
}

static void test_bytes() {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_bytes(1), NULL));

  test_bytes_value(message, "0a00", "");
  test_bytes_value(message, "0a01ff", "ff");
  test_bytes_value(message, "0a04deadbeef", "deadbeef");

  // Second value is the one kept.
  test_bytes_value(message, "0a01de0a01ad", "ad");

  test_decode_error(message, "", "Missing required field value");
  test_decode_error(message, "0800", "Received VARINT for non-VARINT field");
  test_decode_error(message, "090000000000000000",
                    "Received I64 for non-I64 field");
  test_decode_error(message, "0b00", "Unknown wire type");
  test_decode_error(message, "0c00", "Unknown wire type");
  test_decode_error(message, "0d00000000", "Received I32 for non-I32 field");
  test_decode_error(message, "0e00", "Unknown wire type");
  test_decode_error(message, "0f00", "Unknown wire type");

  test_decode_error(message, "0a03", "Insufficient space for LEN data");
  test_decode_error(message, "0a03ffff", "Insufficient space for LEN data");

  UtObjectRef optional_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_optional_bytes(1), NULL));
  test_bytes_value(optional_message, "", "");
  test_bytes_value(optional_message, "0a01ff", "ff");
  test_bytes_value(optional_message, "0a01de0a01ad", "ad");

  UtObjectRef repeated_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value", ut_protobuf_message_field_new_repeated_bytes(1), NULL));
  test_repeated_bytes_value(repeated_message, "", ut_list_new());
  test_repeated_bytes_value(repeated_message, "0a01ff",
                            ut_list_new_from_elements_take(
                                ut_uint8_list_new_from_hex_string("ff"), NULL));
  test_repeated_bytes_value(repeated_message, "0a01de0a01ad",
                            ut_list_new_from_elements_take(
                                ut_uint8_list_new_from_hex_string("de"),
                                ut_uint8_list_new_from_hex_string("ad"), NULL));
}

static void test_enum_value(UtObject *message, const char *hex_string,
                            const char *value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_implements_string(v));
  ut_assert_cstring_equal(ut_string_get_text(v), value);
}

static void test_repeated_enum_value(UtObject *message, const char *hex_string,
                                     UtObject *value) {
  UtObjectRef message_value = test_decode(message, hex_string);
  UtObject *v = ut_map_lookup_string(message_value, "value");
  ut_assert_non_null_object(v);
  ut_assert_true(ut_object_implements_string_list(v));
  ut_assert_equal(v, value);
  ut_object_unref(value);
}

static void test_enum() {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value",
          ut_protobuf_message_field_new_take(
              ut_protobuf_enum_type_new_take(
                  ut_map_new_string_from_elements_take(
                      "ZERO", ut_int32_new(0), "ONE", ut_int32_new(1), "TWO",
                      ut_int32_new(2), NULL)),
              1),
          NULL));

  test_enum_value(message, "0801", "ONE");
  test_enum_value(message, "0802", "TWO");
  test_enum_value(message, "082a", "42");

  // Second value is the one kept.
  test_enum_value(message, "08010802", "TWO");

  test_decode_error(message, "", "Missing required field value");
  test_decode_error(message, "090000000000000000",
                    "Received I64 for non-I64 field");
  test_decode_error(message, "0a00", "Received LEN for non-LEN field");
  test_decode_error(message, "0b00", "Unknown wire type");
  test_decode_error(message, "0c00", "Unknown wire type");
  test_decode_error(message, "0d00000000", "Received I32 for non-I32 field");
  test_decode_error(message, "0e00", "Unknown wire type");
  test_decode_error(message, "0f00", "Unknown wire type");

  test_decode_error(message, "088080808010",
                    "VARINT too large for signed 32 bit value");
  test_decode_error(message, "0880808080808080808001",
                    "VARINT too large for signed 32 bit value");

  UtObjectRef optional_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value",
          ut_protobuf_message_field_new_optional_take(
              ut_protobuf_enum_type_new_take(
                  ut_map_new_string_from_elements_take(
                      "ZERO", ut_int32_new(0), "ONE", ut_int32_new(1), "TWO",
                      ut_int32_new(2), NULL)),
              1),
          NULL));
  test_enum_value(optional_message, "", "ZERO");
  test_enum_value(optional_message, "0801", "ONE");
  test_enum_value(optional_message, "08010802", "TWO");

  UtObjectRef repeated_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "value",
          ut_protobuf_message_field_new_repeated_take(
              ut_protobuf_enum_type_new_take(
                  ut_map_new_string_from_elements_take(
                      "ZERO", ut_int32_new(0), "ONE", ut_int32_new(1), "TWO",
                      ut_int32_new(2), NULL)),
              1),
          NULL));
  test_repeated_enum_value(repeated_message, "", ut_string_list_new());
  test_repeated_enum_value(repeated_message, "0801",
                           ut_string_list_new_from_elements("ONE", NULL));
  test_repeated_enum_value(
      repeated_message, "08010802",
      ut_string_list_new_from_elements("ONE", "TWO", NULL));
}

static void test_message() {
  UtObjectRef message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "name", ut_protobuf_message_field_new_string(1), "age",
          ut_protobuf_message_field_new_uint32(2), NULL));

  // Data in order name, age.
  UtObjectRef data1 =
      ut_uint8_list_new_from_hex_string("0a0b4172746875722044656e74102a");
  UtObjectRef decoder1 = ut_protobuf_decoder_new(data1);
  UtObjectRef value1 = ut_protobuf_decoder_decode_message(decoder1, message);
  ut_assert_null_object(ut_protobuf_decoder_get_error(decoder1));
  UtObject *v1a = ut_map_lookup_string(value1, "name");
  ut_assert_non_null_object(v1a);
  ut_assert_true(ut_object_implements_string(v1a));
  ut_assert_cstring_equal(ut_string_get_text(v1a), "Arthur Dent");
  UtObject *v1b = ut_map_lookup_string(value1, "age");
  ut_assert_non_null_object(v1b);
  ut_assert_true(ut_object_is_uint32(v1b));
  ut_assert_int_equal(ut_uint32_get_value(v1b), 42);

  // Data in order age, name.
  UtObjectRef data2 =
      ut_uint8_list_new_from_hex_string("102a0a0b4172746875722044656e74");
  UtObjectRef decoder2 = ut_protobuf_decoder_new(data2);
  UtObjectRef value2 = ut_protobuf_decoder_decode_message(decoder2, message);
  ut_assert_null_object(ut_protobuf_decoder_get_error(decoder2));
  UtObject *v2a = ut_map_lookup_string(value2, "name");
  ut_assert_non_null_object(v2a);
  ut_assert_true(ut_object_implements_string(v2a));
  ut_assert_cstring_equal(ut_string_get_text(v2a), "Arthur Dent");
  UtObject *v2b = ut_map_lookup_string(value2, "age");
  ut_assert_non_null_object(v2b);
  ut_assert_true(ut_object_is_uint32(v2b));
  ut_assert_int_equal(ut_uint32_get_value(v2b), 42);

  // Duplicate field - age (99), name, age (42).
  UtObjectRef data3 =
      ut_uint8_list_new_from_hex_string("10630a0b4172746875722044656e74102a");
  UtObjectRef decoder3 = ut_protobuf_decoder_new(data3);
  UtObjectRef value3 = ut_protobuf_decoder_decode_message(decoder3, message);
  ut_assert_null_object(ut_protobuf_decoder_get_error(decoder3));
  UtObject *v3a = ut_map_lookup_string(value3, "name");
  ut_assert_non_null_object(v3a);
  ut_assert_true(ut_object_implements_string(v3a));
  ut_assert_cstring_equal(ut_string_get_text(v3a), "Arthur Dent");
  UtObject *v3b = ut_map_lookup_string(value3, "age");
  ut_assert_non_null_object(v3b);
  ut_assert_true(ut_object_is_uint32(v3b));
  ut_assert_int_equal(ut_uint32_get_value(v3b), 42);

  // Empty data.
  test_decode_error(message, "", "Missing required field name");

  // Missing name.
  test_decode_error(message, "102a", "Missing required field name");

  // Missing age.
  test_decode_error(message, "0a0b4172746875722044656e74",
                    "Missing required field age");

  UtObjectRef optional_message =
      ut_protobuf_message_type_new_take(ut_map_new_string_from_elements_take(
          "name", ut_protobuf_message_field_new_string(1), "age",
          ut_protobuf_message_field_new_optional_uint32(2), NULL));

  // All values.
  UtObjectRef data4 =
      ut_uint8_list_new_from_hex_string("0a0b4172746875722044656e74102a");
  UtObjectRef decoder4 = ut_protobuf_decoder_new(data4);
  UtObjectRef value4 =
      ut_protobuf_decoder_decode_message(decoder4, optional_message);
  ut_assert_null_object(ut_protobuf_decoder_get_error(decoder4));
  UtObject *v4a = ut_map_lookup_string(value4, "name");
  ut_assert_non_null_object(v4a);
  ut_assert_true(ut_object_implements_string(v4a));
  ut_assert_cstring_equal(ut_string_get_text(v4a), "Arthur Dent");
  UtObject *v4b = ut_map_lookup_string(value4, "age");
  ut_assert_non_null_object(v4b);
  ut_assert_true(ut_object_is_uint32(v4b));
  ut_assert_int_equal(ut_uint32_get_value(v4b), 42);

  // Missing age (defaults to 0).
  UtObjectRef data5 =
      ut_uint8_list_new_from_hex_string("0a0b4172746875722044656e74");
  UtObjectRef decoder5 = ut_protobuf_decoder_new(data5);
  UtObjectRef value5 =
      ut_protobuf_decoder_decode_message(decoder5, optional_message);
  ut_assert_null_object(ut_protobuf_decoder_get_error(decoder5));
  UtObject *v5a = ut_map_lookup_string(value5, "name");
  ut_assert_non_null_object(v5a);
  ut_assert_true(ut_object_implements_string(v5a));
  ut_assert_cstring_equal(ut_string_get_text(v5a), "Arthur Dent");
  UtObject *v5b = ut_map_lookup_string(value5, "age");
  ut_assert_non_null_object(v5b);
  ut_assert_true(ut_object_is_uint32(v5b));
  ut_assert_int_equal(ut_uint32_get_value(v5b), 0);

  // Empty data.
  test_decode_error(optional_message, "", "Missing required field name");

  // Missing name.
  test_decode_error(optional_message, "102a", "Missing required field name");
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
