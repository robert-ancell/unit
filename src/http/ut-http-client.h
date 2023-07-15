#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef void (*UtHttpResponseCallback)(UtObject *object, UtObject *response);

UtObject *ut_http_client_new();

void ut_http_client_send_request(UtObject *object, const char *method,
                                 const char *uri, UtObject *body,
                                 UtObject *callback_object,
                                 UtHttpResponseCallback callback);

bool ut_object_is_http_client(UtObject *object);
