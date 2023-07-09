#include <stdio.h>

#include "ut.h"

// Handle DNS requests and send back.
static size_t dns_read_cb(UtObject *object, UtObject *datagrams,
                          bool complete) {
  UtObject *socket = object;

  UtObjectRef datagram = ut_list_get_element(datagrams, 0);
  UtObject *request = ut_udp_datagram_get_data(datagram);
  uint16_t id = ut_uint8_list_get_uint16_be(request, 0);

  // Canned response to request for example.com
  UtObjectRef reply = ut_uint8_array_new();
  ut_uint8_list_append_uint16_be(reply, id);
  ut_uint8_list_append_uint16_be(reply, 0xf0); // flags
  ut_uint8_list_append_uint16_be(reply, 0);
  ut_uint8_list_append_uint16_be(reply, 1);
  ut_uint8_list_append_uint16_be(reply, 0);
  ut_uint8_list_append_uint16_be(reply, 0);
  ut_uint8_list_append(reply, 7); // name
  ut_uint8_list_append(reply, 'e');
  ut_uint8_list_append(reply, 'x');
  ut_uint8_list_append(reply, 'a');
  ut_uint8_list_append(reply, 'm');
  ut_uint8_list_append(reply, 'p');
  ut_uint8_list_append(reply, 'l');
  ut_uint8_list_append(reply, 'e');
  ut_uint8_list_append(reply, 3);
  ut_uint8_list_append(reply, 'c');
  ut_uint8_list_append(reply, 'o');
  ut_uint8_list_append(reply, 'm');
  ut_uint8_list_append(reply, 0);
  ut_uint8_list_append_uint16_be(reply, 1);  // type
  ut_uint8_list_append_uint16_be(reply, 1);  // class
  ut_uint8_list_append_uint32_be(reply, 60); // ttl
  ut_uint8_list_append_uint16_be(reply, 4);  // data_length
  ut_uint8_list_append(reply, 93);           // IP address
  ut_uint8_list_append(reply, 184);
  ut_uint8_list_append(reply, 216);
  ut_uint8_list_append(reply, 34);

  ut_udp_socket_send(socket, ut_udp_datagram_get_address(datagram),
                     ut_udp_datagram_get_port(datagram), reply);
  return ut_list_get_length(datagrams);
}

static void lookup_cb(UtObject *object, UtObject *address) {
  ut_cstring_ref address_string = ut_ip_address_to_string(address);

  ut_assert_cstring_equal(address_string, "93.184.216.34");

  ut_event_loop_return(NULL);
}

int main(int argc, char **argv) {
  // Make a mock DNS server
  UtObjectRef dns_socket = ut_udp_socket_new_ipv4();
  ut_input_stream_read(dns_socket, dns_socket, dns_read_cb);
  ut_udp_socket_bind(dns_socket, 0);
  uint16_t dns_port = ut_udp_socket_get_port(dns_socket);

  UtObjectRef address = ut_ipv4_address_new_loopback();
  UtObjectRef client = ut_dns_client_new(address, dns_port);

  UtObjectRef dummy_object = ut_null_new();
  ut_dns_client_lookup_ipv4(client, "example.com", dummy_object, lookup_cb);

  ut_event_loop_run();

  return 0;
}
