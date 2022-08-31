#include <stdio.h>

#include "ut.h"

static size_t http_read_cb(void *user_data, UtObject *data, bool complete) {
  UtObject *socket = user_data;
  UtObjectRef text = ut_string_new_from_utf8(data);

  UtObjectRef reply = ut_string_new("");
  ut_string_append_printf(reply, "HTTP/1.1 200 OK\r\n");
  ut_string_append_printf(reply, "Content-Type: application/json\r\n");
  ut_string_append_printf(reply, "Content-Length: 24\r\n");
  ut_string_append_printf(reply, "\r\n");
  ut_string_append_printf(reply, "{\"text\": \"Hello World!\"}");
  UtObjectRef reply_data = ut_string_get_utf8(reply);
  ut_tcp_socket_send(socket, reply_data);

  return ut_list_get_length(data);
}

static void http_listen_cb(void *user_data, UtObject *socket) {
  ut_input_stream_read(socket, http_read_cb, ut_object_ref(socket), NULL);
}

static size_t read_cb(void *user_data, UtObject *data, bool complete) {
  UtObjectRef text = ut_string_new_from_utf8(data);

  ut_assert_cstring_equal(ut_string_get_text(text),
                          "{\"text\": \"Hello World!\"}");

  ut_event_loop_return(NULL);
  return ut_list_get_length(data);
}

static void http_response_cb(void *user_data, UtObject *response) {
  ut_assert_int_equal(ut_http_response_get_status_code(response), 200);
  ut_assert_cstring_equal(ut_http_response_get_reason_phrase(response), "OK");
  UtObject *headers = ut_http_response_get_headers(response);
  const char *content_type = NULL;
  for (size_t i = 0; i < ut_list_get_length(headers); i++) {
    UtObject *header = ut_object_list_get_element(headers, i);
    if (ut_cstring_equal(ut_http_header_get_name(header), "Content-Type")) {
      content_type = ut_http_header_get_value(header);
    }
  }
  ut_assert_cstring_equal(content_type, "application/json");
  ut_input_stream_read_all(response, read_cb, NULL, NULL);
}

int main(int argc, char **argv) {
  // Set up a test HTTP server
  UtObjectRef http_socket = ut_tcp_server_socket_new_ipv4(0);
  ut_tcp_server_socket_listen(http_socket, http_listen_cb, NULL, NULL);
  uint16_t http_port = ut_tcp_server_socket_get_port(http_socket);

  UtObjectRef http_client = ut_http_client_new();
  ut_cstring_ref uri = ut_cstring_new_printf("http://127.0.0.1:%d", http_port);
  ut_http_client_send_request(http_client, "GET", uri, http_response_cb, NULL,
                              NULL);

  ut_event_loop_run();

  return 0;
}
