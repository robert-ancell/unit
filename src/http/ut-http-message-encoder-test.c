#include <stdio.h>

#include "ut-http-message-encoder.h"
#include "ut.h"

static void check_encoded_data(UtObject *encoded_data,
                               const char *expected_data) {
  UtObjectRef encoded_data_string = ut_string_new_from_utf8(encoded_data);
  ut_assert_is_not_error(encoded_data_string);
  ut_assert_cstring_equal(ut_string_get_text(encoded_data_string),
                          expected_data);
}

static void test_encode_request(const char *method, const char *path,
                                UtObject *headers, const char *data,
                                const char *expected_data) {
  UtObjectRef encoded_data = ut_uint8_list_new();
  UtObjectRef headers_ref =
      headers != NULL ? ut_object_ref(headers) : ut_list_new();
  UtObjectRef data_string = ut_string_new(data);
  UtObjectRef data_utf8 = ut_string_get_utf8(data_string);
  UtObjectRef data_stream = ut_list_input_stream_new(data_utf8);
  UtObjectRef encoder = ut_http_message_encoder_new_request(
      encoded_data, method, path, headers_ref, data_stream);
  ut_http_message_encoder_encode(encoder);

  check_encoded_data(encoded_data, expected_data);
}

static void test_encode_response(unsigned int status_code,
                                 const char *reason_phrase, UtObject *headers,
                                 const char *data, const char *expected_data) {
  UtObjectRef encoded_data = ut_uint8_list_new();
  UtObjectRef headers_ref =
      headers != NULL ? ut_object_ref(headers) : ut_list_new();
  UtObjectRef data_string = ut_string_new(data);
  UtObjectRef data_utf8 = ut_string_get_utf8(data_string);
  UtObjectRef data_stream = ut_list_input_stream_new(data_utf8);
  UtObjectRef encoder = ut_http_message_encoder_new_response(
      encoded_data, status_code, reason_phrase, headers_ref, data_stream);
  ut_http_message_encoder_encode(encoder);

  check_encoded_data(encoded_data, expected_data);
}

static void test_request_line() {
  test_encode_request("GET", "/", NULL, "",
                      "GET / HTTP/1.1\r\n"
                      "\r\n");

  test_encode_request("POST", "/foo", NULL, "",
                      "POST /foo HTTP/1.1\r\n"
                      "\r\n");
}

static void test_response_line() {
  test_encode_response(200, "OK", NULL, "",
                       "HTTP/1.1 200 OK\r\n"
                       "\r\n");

  test_encode_response(404, "Not Found", NULL, "",
                       "HTTP/1.1 404 Not Found\r\n"
                       "\r\n");
}

static void test_headers() {
  UtObjectRef single_headers = ut_list_new_from_elements_take(
      ut_http_header_new("Host", "example.com"), NULL);
  test_encode_request("GET", "/", single_headers, "",
                      "GET / HTTP/1.1\r\n"
                      "Host: example.com\r\n"
                      "\r\n");

  UtObjectRef multiple_headers = ut_list_new_from_elements_take(
      ut_http_header_new("Host", "example.com"),
      ut_http_header_new("Content-Type", "text/plain"), NULL);
  test_encode_request("GET", "/", multiple_headers, "",
                      "GET / HTTP/1.1\r\n"
                      "Host: example.com\r\n"
                      "Content-Type: text/plain\r\n"
                      "\r\n");
}

static void test_body() {
  test_encode_request("GET", "/", NULL, "Hello World!",
                      "GET / HTTP/1.1\r\n"
                      "\r\n"
                      "Hello World!");

  UtObjectRef content_length_headers = ut_list_new_from_elements_take(
      ut_http_header_new("Content-Length", "10"), NULL);
  test_encode_response(200, "OK", content_length_headers, "0123456789abcdef",
                       "HTTP/1.1 200 OK\r\n"
                       "Content-Length: 10\r\n"
                       "\r\n"
                       "0123456789");

  UtObjectRef empty_content_length_headers = ut_list_new_from_elements_take(
      ut_http_header_new("Content-Length", "0"), NULL);
  test_encode_response(200, "OK", empty_content_length_headers, "",
                       "HTTP/1.1 200 OK\r\n"
                       "Content-Length: 0\r\n"
                       "\r\n");

  UtObjectRef chunked_headers = ut_list_new_from_elements_take(
      ut_http_header_new("Transfer-Encoding", "chunked"), NULL);
  test_encode_response(200, "OK", chunked_headers, "Hello World!",
                       "HTTP/1.1 200 OK\r\n"
                       "Transfer-Encoding: chunked\r\n"
                       "\r\n"
                       "c\r\n"
                       "Hello World!\r\n"
                       "0\r\n"
                       "\r\n");

  // FIXME: Multiple chunks

  UtObjectRef empty_chunked_headers = ut_list_new_from_elements_take(
      ut_http_header_new("Transfer-Encoding", "chunked"), NULL);
  test_encode_response(200, "OK", empty_chunked_headers, "",
                       "HTTP/1.1 200 OK\r\n"
                       "Transfer-Encoding: chunked\r\n"
                       "\r\n"
                       "0\r\n"
                       "\r\n");
}

int main(int argc, char **argv) {
  test_request_line();
  test_response_line();
  test_headers();
  test_body();

  return 0;
}
