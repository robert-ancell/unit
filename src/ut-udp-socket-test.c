#include <stdio.h>

#include "ut.h"

// Handle echo requests and send back.
static size_t echo_read_cb(void *user_data, UtObject *datagrams,
                           bool complete) {
  UtObject *socket = user_data;

  size_t datagrams_length = ut_list_get_length(datagrams);
  for (size_t i = 0; i < datagrams_length; i++) {
    UtObjectRef datagram = ut_list_get_element(datagrams, i);
    ut_output_stream_write(socket, datagram);
  }

  return datagrams_length;
}

// Get the response from the echo server
static size_t read_cb(void *user_data, UtObject *datagrams, bool complete) {
  ut_assert_int_equal(ut_list_get_length(datagrams), 1);
  UtObjectRef datagram = ut_list_get_element(datagrams, 0);
  UtObject *data = ut_udp_datagram_get_data(datagram);

  ut_assert_uint8_list_equal_hex(data, "0123456789abcdef");

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
  ut_udp_socket_send(udp_socket, address, echo_port, data);

  ut_event_loop_run();

  return 0;
}
