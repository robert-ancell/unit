#include <stdio.h>

#include "ut-http-message-decoder.h"
#include "ut.h"

static void check_body(UtObject *body, const char *expected_body_text) {
  UtObjectRef body_string = ut_string_new_from_utf8(body);
  ut_assert_is_not_error(body_string);
  ut_assert_cstring_equal(ut_string_get_text(body_string), expected_body_text);
}

static UtObject *decode_request(const char *data_text) {
  UtObjectRef data_string = ut_string_new(data_text);
  UtObjectRef data = ut_string_get_utf8(data_string);
  UtObjectRef data_stream = ut_list_input_stream_new(data);
  UtObjectRef decoder = ut_http_message_decoder_new_request(data_stream);
  ut_http_message_decoder_read(decoder);

  return ut_object_ref(decoder);
}

static UtObject *test_decode_request(const char *data_text,
                                     const char *expected_method,
                                     const char *expected_path,
                                     const char *expected_body_text) {
  UtObjectRef decoder = decode_request(data_text);

  ut_assert_null(ut_http_message_decoder_get_error(decoder));
  ut_assert_cstring_equal(ut_http_message_decoder_get_method(decoder),
                          expected_method);
  ut_assert_cstring_equal(ut_http_message_decoder_get_path(decoder),
                          expected_path);
  UtObjectRef body =
      ut_input_stream_read_sync(ut_http_message_decoder_get_body(decoder));
  check_body(body, expected_body_text);

  return ut_object_ref(ut_http_message_decoder_get_headers(decoder));
}

static UtObject *decode_response(const char *data_text) {
  UtObjectRef data_string = ut_string_new(data_text);
  UtObjectRef data = ut_string_get_utf8(data_string);
  UtObjectRef data_stream = ut_list_input_stream_new(data);
  UtObjectRef decoder = ut_http_message_decoder_new_response(data_stream);
  ut_http_message_decoder_read(decoder);

  return ut_object_ref(decoder);
}

static UtObject *test_decode_response(const char *data_text,
                                      unsigned int expected_status_code,
                                      const char *expected_reason_phrase,
                                      const char *expected_body_text) {
  UtObjectRef decoder = decode_response(data_text);

  ut_assert_null(ut_http_message_decoder_get_error(decoder));
  ut_assert_int_equal(ut_http_message_decoder_get_status_code(decoder),
                      expected_status_code);
  ut_assert_cstring_equal(ut_http_message_decoder_get_reason_phrase(decoder),
                          expected_reason_phrase);
  UtObjectRef body =
      ut_input_stream_read_sync(ut_http_message_decoder_get_body(decoder));
  check_body(body, expected_body_text);

  return ut_object_ref(ut_http_message_decoder_get_headers(decoder));
}

static void test_request_line() {
  UtObjectRef root_headers = test_decode_request("GET / HTTP/1.1\r\n"
                                                 "\r\n",
                                                 "GET", "/", "");
  ut_assert_int_equal(ut_list_get_length(root_headers), 0);

  UtObjectRef path1_headers = test_decode_request("POST /foo HTTP/1.1\r\n"
                                                  "\r\n",
                                                  "POST", "/foo", "");
  ut_assert_int_equal(ut_list_get_length(path1_headers), 0);

  UtObjectRef path2_headers =
      test_decode_request("PUT /foo/bar/baz HTTP/1.1\r\n"
                          "\r\n",
                          "PUT", "/foo/bar/baz", "");
  ut_assert_int_equal(ut_list_get_length(path2_headers), 0);

  UtObjectRef empty_decoder = decode_request("");
  ut_assert_is_error_with_description(
      ut_http_message_decoder_get_error(empty_decoder),
      "Incomplete HTTP message");

  UtObjectRef empty_line_decoder = decode_request("\r\n");
  ut_assert_is_error_with_description(
      ut_http_message_decoder_get_error(empty_line_decoder),
      "Invalid HTTP request line");

  UtObjectRef missing_method_decoder = decode_request("/ HTTP/1.1\r\n"
                                                      "\r\n");
  ut_assert_is_error_with_description(
      ut_http_message_decoder_get_error(missing_method_decoder),
      "Invalid HTTP request line");

  UtObjectRef missing_path_decoder = decode_request("GET HTTP/1.1\r\n"
                                                    "\r\n");
  ut_assert_is_error_with_description(
      ut_http_message_decoder_get_error(missing_path_decoder),
      "Invalid HTTP request line");

  UtObjectRef missing_version_decoder = decode_request("GET /\r\n"
                                                       "\r\n");
  ut_assert_is_error_with_description(
      ut_http_message_decoder_get_error(missing_version_decoder),
      "Invalid HTTP request line");

  UtObjectRef invalid_version_decoder = decode_request("GET / HTTP/8.2\r\n"
                                                       "\r\n");
  ut_assert_is_error_with_description(
      ut_http_message_decoder_get_error(invalid_version_decoder),
      "Invalid HTTP version");

  UtObjectRef invalid_newline_decoder = decode_request("GET / HTTP/1.1\n"
                                                       "\n");
  ut_assert_is_error_with_description(
      ut_http_message_decoder_get_error(invalid_newline_decoder),
      "Incomplete HTTP message");
}

static void test_response_line() {
  UtObjectRef ok_headers = test_decode_response("HTTP/1.1 200 OK\r\n"
                                                "\r\n",
                                                200, "OK", "");
  ut_assert_int_equal(ut_list_get_length(ok_headers), 0);

  UtObjectRef not_found_headers =
      test_decode_response("HTTP/1.1 404 Not Found\r\n"
                           "\r\n",
                           404, "Not Found", "");
  ut_assert_int_equal(ut_list_get_length(ok_headers), 0);

  UtObjectRef empty_decoder = decode_response("");
  ut_assert_is_error_with_description(
      ut_http_message_decoder_get_error(empty_decoder),
      "Incomplete HTTP message");

  UtObjectRef empty_line_decoder = decode_response("\r\n");
  ut_assert_is_error_with_description(
      ut_http_message_decoder_get_error(empty_line_decoder),
      "Invalid HTTP status line");

  UtObjectRef missing_status_decoder = decode_response("HTTP/1.1 OK\r\n"
                                                       "\r\n");
  ut_assert_is_error_with_description(
      ut_http_message_decoder_get_error(missing_status_decoder),
      "Invalid HTTP status line");

  UtObjectRef missing_reason_phrase_decoder = decode_response("HTTP/1.1 200\r\n"
                                                              "\r\n");
  ut_assert_is_error_with_description(
      ut_http_message_decoder_get_error(missing_reason_phrase_decoder),
      "Invalid HTTP status line");

  UtObjectRef missing_version_decoder = decode_response("200 OK\r\n"
                                                        "\r\n");
  ut_assert_is_error_with_description(
      ut_http_message_decoder_get_error(missing_version_decoder),
      "Invalid HTTP version");

  UtObjectRef invalid_version_decoder = decode_response("HTTP/8.2 200 OK\r\n"
                                                        "\r\n");
  ut_assert_is_error_with_description(
      ut_http_message_decoder_get_error(invalid_version_decoder),
      "Invalid HTTP version");

  UtObjectRef invalid_newline_decoder = decode_response("HTTP/1.1 200 OK\n"
                                                        "\n");
  ut_assert_is_error_with_description(
      ut_http_message_decoder_get_error(invalid_newline_decoder),
      "Incomplete HTTP message");
}

static void test_headers() {
  UtObjectRef single_headers = test_decode_request("GET / HTTP/1.1\r\n"
                                                   "Host: example.com\r\n"
                                                   "\r\n",
                                                   "GET", "/", "");
  ut_assert_int_equal(ut_list_get_length(single_headers), 1);
  UtObject *header = ut_object_list_get_element(single_headers, 0);
  ut_assert_cstring_equal(ut_http_header_get_name(header), "Host");
  ut_assert_cstring_equal(ut_http_header_get_value(header), "example.com");

  UtObjectRef multiple_headers =
      test_decode_request("GET / HTTP/1.1\r\n"
                          "Host: example.com\r\n"
                          "Content-Type: text/plain\r\n"
                          "\r\n",
                          "GET", "/", "");
  ut_assert_int_equal(ut_list_get_length(multiple_headers), 2);
  header = ut_object_list_get_element(multiple_headers, 0);
  ut_assert_cstring_equal(ut_http_header_get_name(header), "Host");
  ut_assert_cstring_equal(ut_http_header_get_value(header), "example.com");
  header = ut_object_list_get_element(multiple_headers, 1);
  ut_assert_cstring_equal(ut_http_header_get_name(header), "Content-Type");
  ut_assert_cstring_equal(ut_http_header_get_value(header), "text/plain");
}

static void test_body() {
  UtObjectRef eof_headers = test_decode_request("GET / HTTP/1.1\r\n"
                                                "\r\n"
                                                "Hello World!",
                                                "GET", "/", "Hello World!");
  ut_assert_non_null(eof_headers);

  UtObjectRef content_length_headers =
      test_decode_response("HTTP/1.1 200 OK\r\n"
                           "Content-Length: 10\r\n"
                           "\r\n"
                           "0123456789abcdef",
                           200, "OK", "0123456789");
  ut_assert_non_null(content_length_headers);

  UtObjectRef empty_content_length_headers =
      test_decode_response("HTTP/1.1 200 OK\r\n"
                           "Content-Length: 0\r\n"
                           "\r\n",
                           200, "OK", "");
  ut_assert_non_null(empty_content_length_headers);

  UtObjectRef missing_content_decoder = decode_request("GET / HTTP/1.1\r\n"
                                                       "Content-Length: 10\r\n"
                                                       "\r\n"
                                                       "012345678");
  ut_assert_is_error_with_description(
      ut_http_message_decoder_get_error(missing_content_decoder),
      "Incomplete HTTP message");

  UtObjectRef chunked_headers =
      test_decode_response("HTTP/1.1 200 OK\r\n"
                           "Transfer-Encoding: chunked\r\n"
                           "\r\n"
                           "5\r\n"
                           "life \r\n"
                           "14\r\n"
                           "is more complicated \r\n"
                           "a\r\n"
                           "in chunks.\r\n"
                           "0\r\n"
                           "\r\n",
                           200, "OK", "life is more complicated in chunks.");
  ut_assert_non_null(chunked_headers);

  UtObjectRef empty_chunked_headers =
      test_decode_response("HTTP/1.1 200 OK\r\n"
                           "Transfer-Encoding: chunked\r\n"
                           "\r\n"
                           "0\r\n"
                           "\r\n",
                           200, "OK", "");
  ut_assert_non_null(empty_chunked_headers);

  UtObjectRef missing_chunks_decoder =
      decode_request("GET / HTTP/1.1\r\n"
                     "Transfer-Encoding: chunked\r\n"
                     "\r\n");
  ut_assert_is_error_with_description(
      ut_http_message_decoder_get_error(missing_chunks_decoder),
      "Incomplete HTTP message");

  UtObjectRef missing_final_chunk_decoder =
      decode_request("GET / HTTP/1.1\r\n"
                     "Transfer-Encoding: chunked\r\n"
                     "\r\n"
                     "3\r\n"
                     "one\r\n"
                     "3\r\n"
                     "two\r\n");
  ut_assert_is_error_with_description(
      ut_http_message_decoder_get_error(missing_final_chunk_decoder),
      "Incomplete HTTP message");

  UtObjectRef missing_final_chunk_data_decoder =
      decode_request("GET / HTTP/1.1\r\n"
                     "Transfer-Encoding: chunked\r\n"
                     "\r\n"
                     "3\r\n"
                     "one\r\n"
                     "3\r\n"
                     "two\r\n"
                     "0\r\n");
  ut_assert_is_error_with_description(
      ut_http_message_decoder_get_error(missing_final_chunk_data_decoder),
      "Incomplete HTTP message");

  UtObjectRef invalid_chunk_header_newline_decoder =
      decode_request("GET / HTTP/1.1\r\n"
                     "Transfer-Encoding: chunked\r\n"
                     "\r\n"
                     "3\r\n"
                     "one\r\n"
                     "3\n"
                     "two\r\n"
                     "0\r\n"
                     "\r\n");
  ut_assert_is_error_with_description(
      ut_http_message_decoder_get_error(invalid_chunk_header_newline_decoder),
      "Incomplete HTTP message");

  UtObjectRef invalid_chunk_data_newline_decoder =
      decode_request("GET / HTTP/1.1\r\n"
                     "Transfer-Encoding: chunked\r\n"
                     "\r\n"
                     "3\r\n"
                     "onexx"
                     "3\n"
                     "two\r\n"
                     "0\r\n"
                     "\r\n");
  ut_assert_is_error_with_description(
      ut_http_message_decoder_get_error(invalid_chunk_data_newline_decoder),
      "Invalid HTTP chunk");
}

int main(int argc, char **argv) {
  test_request_line();
  test_response_line();
  test_headers();
  test_body();

  return 0;
}
