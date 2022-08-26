#include <stdio.h>

#include "ut.h"

// Echo all sent data.
static size_t echo_read_cb(void *user_data, UtObject *data, bool complete) {
  UtObject *socket = user_data;
  ut_tcp_socket_send(socket, data);
  return ut_list_get_length(data);
}

static void echo_listen_cb(void *user_data, UtObject *socket) {
  ut_input_stream_read(socket, echo_read_cb, ut_object_ref(socket), NULL);
}

// Get the response from the echo server
static size_t read_cb(void *user_data, UtObject *data, bool complete) {
  uint8_t expected_data[8] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};
  ut_assert_uint8_list_equal(data, expected_data, 8);

  ut_event_loop_return(NULL);

  return ut_list_get_length(data);
}

static void connect_cb(void *user_data) {
  UtObject *socket = user_data;

  ut_input_stream_read(socket, read_cb, NULL, NULL);

  // Send a message.
  UtObjectRef data = ut_uint8_array_new_from_elements(8, 0x01, 0x23, 0x45, 0x67,
                                                      0x89, 0xab, 0xcd, 0xef);
  ut_tcp_socket_send(socket, data);
}

int main(int argc, char **argv) {
  // Set up a socket that echos back requests.
  UtObjectRef echo_socket = ut_tcp_server_socket_new_ipv4(0);
  ut_tcp_server_socket_listen(echo_socket, echo_listen_cb, NULL, NULL);
  uint16_t echo_port = ut_tcp_server_socket_get_port(echo_socket);

  // Create a socket to send to the echo port.
  UtObjectRef address = ut_ipv4_address_new_loopback();
  UtObjectRef socket = ut_tcp_socket_new(address, echo_port);
  ut_tcp_socket_connect(socket, connect_cb, socket, NULL);

  ut_event_loop_run();

  return 0;
}
