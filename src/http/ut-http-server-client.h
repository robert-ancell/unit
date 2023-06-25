#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef void (*UtHttpServerClientRequestCallback)(UtObject *object,
                                                  UtObject *request);

UtObject *ut_http_server_client_new(UtObject *input_stream,
                                    UtObject *callback_object,
                                    UtHttpServerClientRequestCallback callback);

void ut_http_server_client_read(UtObject *object);

void ut_http_server_client_send_response(UtObject *object, UtObject *response);

bool ut_object_is_http_server_client(UtObject *object);
