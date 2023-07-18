#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ut.h"

// Echo all sent data.
static size_t echo_read_cb(void *user_data, UtObject *data, bool complete) {
  UtObject *socket = user_data;
  ut_tcp_socket_send(socket, data);
  return ut_list_get_length(data);
}

static void echo_listen_cb(UtObject *object, UtObject *socket) {
  ut_input_stream_read(socket, echo_read_cb, ut_object_ref(socket));
}

// Get the response from the echo server
static size_t read_cb(void *user_data, UtObject *data, bool complete) {
  ut_assert_uint8_list_equal_hex(data, "0123456789abcdef");

  ut_event_loop_return(NULL);

  return ut_list_get_length(data);
}

static void connect_cb(UtObject *object, UtObject *error) {
  UtObject *socket = object;

  ut_assert_null(error);

  ut_input_stream_read(socket, read_cb, NULL);

  // Send a message.
  UtObjectRef data = ut_uint8_array_new_from_hex_string("0123456789abcdef");
  ut_tcp_socket_send(socket, data);
}

int main(int argc, char **argv) {
  char dir[] = "/tmp/ut-test-XXXXXX";
  mkdtemp(dir);
  ut_cstring_ref path = ut_cstring_new_printf("%s/socket", dir);

  // Set up a socket that echos back requests.
  UtObjectRef echo_socket = ut_tcp_server_socket_new_unix(path);
  UtObjectRef dummy_object = ut_null_new();
  ut_assert_true(ut_tcp_server_socket_listen(echo_socket, dummy_object,
                                             echo_listen_cb, NULL));
  uint16_t echo_port = ut_tcp_server_socket_get_port(echo_socket);

  // Create a socket to send to the echo port.
  UtObjectRef address = ut_unix_socket_address_new(path);
  UtObjectRef socket = ut_tcp_socket_new(address, echo_port);
  ut_tcp_socket_connect(socket, socket, connect_cb);

  ut_event_loop_run();

  unlink(path);
  rmdir(dir);

  return 0;
}
