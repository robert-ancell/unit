#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_http_message_encoder_new_request(UtObject *output_stream,
                                              const char *method,
                                              const char *path,
                                              UtObject *headers,
                                              UtObject *body);

UtObject *ut_http_message_encoder_new_response(UtObject *output_stream,
                                               unsigned int status_code,
                                               const char *reason_phrase,
                                               UtObject *headers,
                                               UtObject *body);

void ut_http_message_encoder_encode(UtObject *object);

/// Returns [true] if [object] is a [UtHttpMessageEncoder].
bool ut_object_is_http_message_encoder(UtObject *object);
