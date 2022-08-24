#include <stdio.h>

#include "ut.h"

// Handle echo requests and send back.
static size_t echo_read_cb(void *user_data, UtObject *datagram, bool complete) {
  UtObject *socket = user_data;
  ut_output_stream_write(socket, datagram);
  return 1;
}

// Get the response from the echo server
static size_t read_cb(void *user_data, UtObject *datagram, bool complete) {
  UtObject *data = ut_udp_datagram_get_data(datagram);

  uint8_t expected_data[8] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};
  ut_assert_uint8_list_equal(data, expected_data, 8);

  ut_event_loop_return(NULL);

  return 1;
}

int main(int argc, char **argv) {
  // Set up a socket that echos back requests.
  UtObjectRef echo_socket = ut_udp_socket_new_ipv4();
  ut_input_stream_read(echo_socket, echo_read_cb, echo_socket, NULL);
  ut_udp_socket_bind(echo_socket, 0);
  uint16_t echo_port = ut_udp_socket_get_port(echo_socket);

  // Create a socket to send to the echo port.
  UtObjectRef udp_socket = ut_udp_socket_new_ipv4();
  ut_input_stream_read(udp_socket, read_cb, NULL, NULL);

  // Send a message.
  UtObjectRef address = ut_ipv4_address_new_loopback();
  UtObjectRef data = ut_uint8_array_new_from_elements(8, 0x01, 0x23, 0x45, 0x67,
                                                      0x89, 0xab, 0xcd, 0xef);
  UtObjectRef datagram = ut_udp_datagram_new(address, echo_port, data);
  ut_output_stream_write(udp_socket, datagram);

  ut_event_loop_run();

  return 0;
}
