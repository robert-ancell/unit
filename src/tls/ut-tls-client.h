#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new TLS client.
///
/// !arg-type input_stream UtInputStream
/// !arg-type output_stream UtOutputStream
/// !return-ref
/// !return-type UtTlsClient
UtObject *ut_tls_client_new(UtObject *input_stream, UtObject *output_stream);

/// Creates a new TLS client connected to a server on [address] and TCP [port].
///
/// !arg-type address UtIpv4Address UtIpv6Address UtUnixSocketAddress
/// !return-ref
/// !return-type UtTlsClient
UtObject *ut_tls_client_new_tcp(UtObject *address, uint16_t port);

void ut_tls_client_connect(UtObject *object);

/// Returns [true] if [object] is a [UtTlsClient].
bool ut_object_is_tls_client(UtObject *object);
