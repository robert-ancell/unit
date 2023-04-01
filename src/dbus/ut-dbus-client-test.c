#include <stdlib.h>
#include <unistd.h>

#include "ut.h"

static UtObject *client1 = NULL;
static UtObject *client2 = NULL;

static void client2_echo_cb(void *user_data, UtObject *out_args) {
  ut_assert_int_equal(ut_list_get_length(out_args), 2);
  UtObjectRef arg0 = ut_list_get_element(out_args, 0);
  UtObjectRef arg1 = ut_list_get_element(out_args, 1);
  ut_assert_true(ut_object_implements_string(arg0));
  ut_assert_cstring_equal(ut_string_get_text(arg0), "Lorem Ipsum");
  ut_assert_true(ut_object_is_uint32(arg1));
  ut_assert_float_equal(ut_uint32_get_value(arg1), 999);

  ut_event_loop_return(NULL);
}

static void client1_ping_cb(void *user_data, UtObject *out_args) {
  const char *client1_name = ut_dbus_client_get_unique_name(client1);

  // Call this client from the other client.
  UtObjectRef args = ut_list_new_from_elements_take(
      ut_string_new("Hello World!"), ut_uint32_new(42), NULL);
  ut_dbus_client_call_method(client2, client1_name, "/com/example/Test",
                             "com.example.Test", "Foo", args, client2_echo_cb,
                             NULL, NULL);
}

static void client1_method_call_cb(void *user_data, UtObject *method_call) {
  ut_assert_cstring_equal(ut_dbus_message_get_path(method_call),
                          "/com/example/Test");
  ut_assert_cstring_equal(ut_dbus_message_get_interface(method_call),
                          "com.example.Test");
  ut_assert_cstring_equal(ut_dbus_message_get_member(method_call), "Foo");

  UtObject *args = ut_dbus_message_get_args(method_call);
  ut_assert_int_equal(ut_list_get_length(args), 2);
  UtObjectRef arg0 = ut_list_get_element(args, 0);
  UtObjectRef arg1 = ut_list_get_element(args, 1);
  ut_assert_true(ut_object_implements_string(arg0));
  ut_assert_cstring_equal(ut_string_get_text(arg0), "Hello World!");
  ut_assert_true(ut_object_is_uint32(arg1));
  ut_assert_int_equal(ut_uint32_get_value(arg1), 42);

  UtObjectRef reply_args = ut_list_new_from_elements_take(
      ut_string_new("Lorem Ipsum"), ut_uint32_new(999), NULL);
  ut_dbus_client_send_reply(client1, method_call, reply_args);
}

int main(int argc, char **argv) {
  char dir[] = "/tmp/ut-test-XXXXXX";
  mkdtemp(dir);
  ut_cstring_ref path = ut_cstring_new_printf("%s/bus", dir);

  UtObjectRef server = ut_dbus_server_new();
  ut_dbus_server_listen_unix(server, path);

  ut_cstring_ref address = ut_cstring_new_printf("unix:path=%s", path);
  client1 = ut_dbus_client_new(address);
  ut_dbus_client_set_method_call_handler(client1, client1_method_call_cb, NULL,
                                         NULL);

  client2 = ut_dbus_client_new(address);

  // Connect serving client to the bus
  ut_dbus_client_call_method(client1, "org.freedesktop.DBus",
                             "/org/freedesktop/DBus", "org.freedesktop.DBus",
                             "Ping", NULL, client1_ping_cb, NULL, NULL);

  ut_event_loop_run();

  unlink(path);
  rmdir(dir);

  return 0;
}
