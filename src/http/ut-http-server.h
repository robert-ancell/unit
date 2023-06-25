#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Method to handle a received HTTP [request].
typedef void (*UtHttpServerRequestCallback)(UtObject *object,
                                            UtObject *request);

/// Creates a new HTTP server. Requests are handled in [callback].
///
/// !return-ref
/// !return-type UtHttpServer
UtObject *ut_http_server_new(UtObject *callback_object,
                             UtHttpServerRequestCallback callback);

/// Listens for requests on the IPv4 [port]. If fails returns [false] and sets
/// [error].
bool ut_http_server_listen_ipv4(UtObject *object, uint16_t port,
                                UtObject **error);

/// Listens for requests on any available IPv4 [port]. If fails returns [false]
/// and sets [error].
bool ut_http_server_listen_ipv4_any(UtObject *object, uint16_t *port,
                                    UtObject **error);

/// Listens for requests on the IPv6 [port]. If fails returns [false] and sets
/// [error].
bool ut_http_server_listen_ipv6(UtObject *object, uint16_t port,
                                UtObject **error);

/// Listens for requests on any available IPv6 [port]. If fails returns [false]
/// and sets [error].
bool ut_http_server_listen_ipv6_any(UtObject *object, uint16_t *port,
                                    UtObject **error);

/// Sends the [response] to [request].
///
/// !arg-type request UtHttpRequest
/// !arg-type response UtHttpResponse
void ut_http_server_respond(UtObject *object, UtObject *request,
                            UtObject *response);

/// Returns [true] if [object] is a [UtHttpServer].
bool ut_object_is_http_server(UtObject *object);
