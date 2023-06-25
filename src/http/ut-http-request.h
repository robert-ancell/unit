#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new HTTP request with [method], [path], [headers]
/// and [body].
///
/// !arg-type headers UtList
/// !arg-type body UtUint8List
/// !return-ref
/// !return-type UtHttpRequest
UtObject *ut_http_request_new(const char *method, const char *path,
                              UtObject *headers, UtObject *body);

/// Returns the method in this request, e.g. "GET".
const char *ut_http_request_get_method(UtObject *object);

/// Returns the path in this request, e.g. "/".
const char *ut_http_request_get_path(UtObject *object);

/// Returns the headers ([UtHttpHeader]) in this request.
/// !return-type UtObjectList
UtObject *ut_http_request_get_headers(UtObject *object);

/// Returns the body of this request.
/// !return-type UtUint8List
UtObject *ut_http_request_get_body(UtObject *object);

/// Returns [true] if [object] is a [UtHttpRequest].
bool ut_object_is_http_request(UtObject *object);
