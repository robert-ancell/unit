#include <stdbool.h>
#include <sys/types.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtHttpResponse].
///
/// !arg-type headers UtList
/// !arg-type body UtUint8List
/// !return-ref
UtObject *ut_http_response_new(unsigned int status_code,
                               const char *reason_phrase, UtObject *headers,
                               UtObject *body);

/// Returns the status code in this response, e.g. 200.
unsigned int ut_http_response_get_status_code(UtObject *object);

/// Returns the reason phrase in this response, e.g. "OK".
const char *ut_http_response_get_reason_phrase(UtObject *object);

/// Returns the headers ([UtHttpHeader]) in this response.
/// !return-type UtObjectList
UtObject *ut_http_response_get_headers(UtObject *object);

/// Returns the value of the header with [name] or [NULL] if no header in this
/// response.
const char *ut_http_response_get_header(UtObject *object, const char *name);

/// Returns the number of bytes in the response body or -1 if unknown.
ssize_t ut_http_response_get_content_length(UtObject *object);

/// Returns the body of this response.
/// !return-type UtUint8List
UtObject *ut_http_response_get_body(UtObject *object);

/// Returns [true] if [object] is a [UtHttpResponse].
bool ut_object_is_http_response(UtObject *object);
