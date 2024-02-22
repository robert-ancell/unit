#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Method to handle a received HTTP [response].
typedef void (*UtHttpResponseCallback)(UtObject *object, UtObject *response);

/// Creates a new HTTP client.
///
/// !return-ref
/// !return-type UtHttpClient
UtObject *ut_http_client_new();

/// Sends a request with [method] to [uri] containing [body]. The result is
/// returned in [callback].
///
/// !arg-type body UtUint8List
void ut_http_client_send_request(UtObject *object, const char *method,
                                 const char *uri, UtObject *body,
                                 UtObject *callback_object,
                                 UtHttpResponseCallback callback);

/// Returns [true] if [object] is a [UtHttpClient].
bool ut_object_is_http_client(UtObject *object);
