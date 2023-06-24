#include <stdbool.h>
#include <stdint.h>

#include "ut-http-client.h"
#include "ut-object.h"

#pragma once

UtObject *ut_http_request_new(const char *host, uint16_t port,
                              const char *method, const char *path,
                              UtObject *headers, UtObject *body,
                              UtHttpResponseCallback callback,
                              void *callback_user_data,
                              UtObject *callback_cancel);

const char *ut_http_request_get_host(UtObject *object);

uint16_t ut_http_request_get_port(UtObject *object);

bool ut_object_is_http_request(UtObject *object);
