#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_http_message_decoder_new_request(UtObject *input_stream);

UtObject *ut_http_message_decoder_new_response(UtObject *input_stream);

void ut_http_message_decoder_read(UtObject *object);

const char *ut_http_message_decoder_get_method(UtObject *object);

const char *ut_http_message_decoder_get_path(UtObject *object);

unsigned int ut_http_message_decoder_get_status_code(UtObject *object);

const char *ut_http_message_decoder_get_reason_phrase(UtObject *object);

UtObject *ut_http_message_decoder_get_headers(UtObject *object);

bool ut_http_message_decoder_get_headers_done(UtObject *object);

UtObject *ut_http_message_decoder_get_body(UtObject *object);

bool ut_http_message_decoder_get_done(UtObject *object);

UtObject *ut_http_message_decoder_get_error(UtObject *object);

bool ut_object_is_http_message_decoder(UtObject *object);
