#include <assert.h>

#include "ut.h"

#include "dbus/ut-dbus-message-encoder.h"
#include "dbus/ut-dbus-message.h"

int main(int argc, char **argv) {
  UtObjectRef encoder = ut_dbus_message_encoder_new();

  UtObjectRef method_call_message =
      ut_dbus_message_new_method_call("com.example.Name", "/com/example/Path",
                                      "com.example.Interface1", "Method", NULL);
  UtObjectRef method_call_data =
      ut_dbus_message_encoder_encode(encoder, method_call_message);
  uint8_t expected_method_call_data[] = {
      0x6c, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x69, 0x00, 0x00, 0x00, 0x01, 0x01, 0x6f, 0x00, 0x11, 0x00, 0x00, 0x00,
      0x2f, 0x63, 0x6f, 0x6d, 0x2f, 0x65, 0x78, 0x61, 0x6d, 0x70, 0x6c, 0x65,
      0x2f, 0x50, 0x61, 0x74, 0x68, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x02, 0x01, 0x73, 0x00, 0x16, 0x00, 0x00, 0x00, 0x63, 0x6f, 0x6d, 0x2e,
      0x65, 0x78, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x2e, 0x49, 0x6e, 0x74, 0x65,
      0x72, 0x66, 0x61, 0x63, 0x65, 0x31, 0x00, 0x00, 0x03, 0x01, 0x73, 0x00,
      0x06, 0x00, 0x00, 0x00, 0x4d, 0x65, 0x74, 0x68, 0x6f, 0x64, 0x00, 0x00,
      0x06, 0x01, 0x73, 0x00, 0x10, 0x00, 0x00, 0x00, 0x63, 0x6f, 0x6d, 0x2e,
      0x65, 0x78, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x2e, 0x4e, 0x61, 0x6d, 0x65,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  ut_assert_uint8_list_equal(method_call_data, expected_method_call_data, 128);

  UtObjectRef method_return_message =
      ut_dbus_message_new_method_return(123456, NULL);
  UtObjectRef method_return_data =
      ut_dbus_message_encoder_encode(encoder, method_return_message);
  uint8_t expected_method_return_data[] = {
      0x6c, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x08, 0x00, 0x00, 0x00, 0x05, 0x01, 0x75, 0x00, 0x40, 0xe2, 0x01, 0x00};
  ut_assert_uint8_list_equal(method_return_data, expected_method_return_data,
                             24);

  UtObjectRef error_message =
      ut_dbus_message_new_error("com.example.Error", NULL);
  UtObjectRef error_data =
      ut_dbus_message_encoder_encode(encoder, error_message);
  uint8_t expected_error_data[] = {
      0x6c, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x1a, 0x00, 0x00, 0x00, 0x04, 0x01, 0x73, 0x00, 0x11, 0x00, 0x00, 0x00,
      0x63, 0x6f, 0x6d, 0x2e, 0x65, 0x78, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x2e,
      0x45, 0x72, 0x72, 0x6f, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  ut_assert_uint8_list_equal(error_data, expected_error_data, 48);

  UtObjectRef signal_message = ut_dbus_message_new_signal(
      "com.example.Name", "com.example.Interface1", "Signal1", NULL);
  UtObjectRef signal_data =
      ut_dbus_message_encoder_encode(encoder, signal_message);
  uint8_t expected_signal_data[] = {
      0x6c, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x50, 0x00, 0x00, 0x00, 0x01, 0x01, 0x6f, 0x00, 0x10, 0x00, 0x00, 0x00,
      0x63, 0x6f, 0x6d, 0x2e, 0x65, 0x78, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x2e,
      0x4e, 0x61, 0x6d, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x02, 0x01, 0x73, 0x00, 0x16, 0x00, 0x00, 0x00, 0x63, 0x6f, 0x6d, 0x2e,
      0x65, 0x78, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x2e, 0x49, 0x6e, 0x74, 0x65,
      0x72, 0x66, 0x61, 0x63, 0x65, 0x31, 0x00, 0x00, 0x03, 0x01, 0x73, 0x00,
      0x07, 0x00, 0x00, 0x00, 0x53, 0x69, 0x67, 0x6e, 0x61, 0x6c, 0x31, 0x00};
  ut_assert_uint8_list_equal(signal_data, expected_signal_data, 96);

  return 0;
}
