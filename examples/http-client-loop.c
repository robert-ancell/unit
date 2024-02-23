#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "ut.h"

static UtObjectRef client;
static int counter;
static void response_cb(UtObject *object, UtObject *response);

static size_t body_cb(UtObject *url_obj, UtObject *data, bool complete) {

  UtObjectRef data_string = ut_string_new_from_utf8(data);
  printf("%d %s", counter, ut_string_get_text(data_string));

  counter++;

  if (complete) {
    ut_http_client_send_request(client,
      "GET",
      ut_string_get_text(url_obj),
      NULL,
      url_obj,
      response_cb);
  }

  return ut_list_get_length(data);
}

static void response_cb(UtObject *url_obj, UtObject *response) {
  // You get a system error for "Connection refused"
  if (ut_object_implements_error(response)) {
    fprintf(stderr, "%s\n", ut_error_get_description(response));
    ut_event_loop_return(ut_int32_new(1));
    return;
  }

  int status = ut_http_response_get_status_code(response);
  if (status!= 200) {
    fprintf(stderr, "%s\n", ut_http_response_get_reason_phrase(response));
    UtObjectRef return_code = ut_int32_new(2);
    ut_event_loop_return(return_code);
    return;
  }

  ut_input_stream_read_all(ut_http_response_get_body(response),
                           url_obj,
                           body_cb);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: http [url]\n");
    return 1;
  }
  const char *url = argv[1];

  counter = 0;
  client = ut_http_client_new();

  UtObject *url_obj = ut_string_new(url);
  ut_http_client_send_request(client, "GET", url, NULL,url_obj,
                              response_cb);

  UtObjectRef return_code = ut_event_loop_run();
  return ut_int32_get_value(return_code);
}
