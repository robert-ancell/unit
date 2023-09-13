#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new UDP socket that communicates using IPv4.
///
/// !return-ref
/// !return-type UtUdpSocket
UtObject *ut_udp_socket_new_ipv4();

/// Creates a new UDP socket that communicates using IPv6.
///
/// !return-ref
/// !return-type UtUdpSocket
UtObject *ut_udp_socket_new_ipv6();

/// Bind this socket to the supplied [port] - it will now receive datagrams sent
/// to that port.
void ut_udp_socket_bind(UtObject *object, uint16_t port);

/// Get the port this socket is listening on.
uint16_t ut_udp_socket_get_port(UtObject *object);

/// Send [data] to the host [address]:[port].
///
/// !arg-type address UtIpAddress
/// !arg-type data UtUint8List
void ut_udp_socket_send(UtObject *object, UtObject *address, uint16_t port,
                        UtObject *data);

/// Returns [true] if [object] is a [UtUdpSocket].
bool ut_object_is_udp_socket(UtObject *object);
