#include <stdio.h>

#include "ut.h"

static void request_cb(UtObject *object, UtObject *request) {
  ut_assert_cstring_equal(ut_http_request_get_method(request), "GET");
  ut_assert_cstring_equal(ut_http_request_get_path(request), "/example/path");
  UtObject *headers = ut_http_request_get_headers(request);
  ut_assert_int_equal(ut_list_get_length(headers), 1);
  UtObject *header = ut_object_list_get_element(headers, 0);
  ut_assert_cstring_equal(ut_http_header_get_name(header), "X-Test-Header");
  ut_assert_cstring_equal(ut_http_header_get_value(header),
                          "Test Header Value");

  ut_event_loop_return(NULL);
}

int main(int argc, char **argv) {
  UtObjectRef dummy_object = ut_null_new();

  UtObjectRef http_server = ut_http_server_new(dummy_object, request_cb);
  uint16_t port;
  UtObjectRef error = NULL;
  ut_http_server_listen_ipv4_any(http_server, &port, &error);

  UtObjectRef address = ut_ipv4_address_new_loopback();
  UtObjectRef socket = ut_tcp_socket_new(address, port);
  ut_tcp_socket_connect(socket, dummy_object, NULL);
  UtObjectRef data_string = ut_string_new("GET /example/path HTTP/1.1\r\n"
                                          "X-Test-Header: Test Header Value\r\n"
                                          "\r\n");
  UtObjectRef data_utf8 = ut_string_get_utf8(data_string);
  ut_tcp_socket_send(socket, data_utf8);

  ut_event_loop_run();

  return 0;
}
