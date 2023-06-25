#include <stdio.h>

#include "ut.h"

static UtObject *server = NULL;

static void request_cb(UtObject *object, UtObject *request) {
  UtObjectRef headers = ut_list_new();
  UtObjectRef response = ut_http_response_new(200, "OK", headers, NULL);
  ut_http_server_respond(server, request, response);
}

int main(int argc, char **argv) {
  UtObjectRef dummy_object = ut_null_new();
  server = ut_http_server_new(dummy_object, request_cb);
  UtObjectRef error = NULL;
  if (!ut_http_server_listen_ipv4(server, 8080, &error)) {
    ut_cstring_ref description = ut_error_get_description(error);
    printf("Unable to listen on port: %s\n", description);
    return 1;
  }

  UtObjectRef return_code = ut_event_loop_run();

  ut_object_unref(server);

  return ut_int32_get_value(return_code);
}
