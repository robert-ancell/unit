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
  ut_assert_uint8_list_equal_hex(
      method_call_data,
      "6c01000100000000000000006900000001016f00110000002f636f6d2f6578616d706c65"
      "2f50617468000000000000000201730016000000636f6d2e6578616d706c652e496e7465"
      "726661636531000003017300060000004d6574686f6400000601730010000000636f6d2e"
      "6578616d706c652e4e616d650000000000000000");

  UtObjectRef method_return_message =
      ut_dbus_message_new_method_return(123456, NULL);
  UtObjectRef method_return_data =
      ut_dbus_message_encoder_encode(encoder, method_return_message);
  ut_assert_uint8_list_equal_hex(
      method_return_data, "6c0200010000000000000000080000000501750040e20100");

  UtObjectRef error_message =
      ut_dbus_message_new_error("com.example.Error", NULL);
  UtObjectRef error_data =
      ut_dbus_message_encoder_encode(encoder, error_message);
  ut_assert_uint8_list_equal_hex(
      error_data, "6c03000100000000000000001a0000000401730011000000636f6d2e6578"
                  "616d706c652e4572726f7200000000000000");

  UtObjectRef signal_message = ut_dbus_message_new_signal(
      "com.example.Name", "com.example.Interface1", "Signal1", NULL);
  UtObjectRef signal_data =
      ut_dbus_message_encoder_encode(encoder, signal_message);
  ut_assert_uint8_list_equal_hex(
      signal_data,
      "6c04000100000000000000005000000001016f0010000000636f6d2e6578616d706c652e"
      "4e616d6500000000000000000201730016000000636f6d2e6578616d706c652e496e7465"
      "726661636531000003017300070000005369676e616c3100");

  return 0;
}
