#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Method called when a socket is connected. If the connection has failed
/// [error] contains the error that occurred, otherwise [NULL].
///
/// !arg-type error UtError
typedef void (*UtTcpSocketConnectCallback)(UtObject *object, UtObject *error);

/// Creates a new TCP socket from an existing socket [fd].
///
/// !arg-type fd UtFileDescriptor
/// !return-ref
/// !return-type UtTcpSocket
UtObject *ut_tcp_socket_new_from_fd(UtObject *fd);

/// Creates a new TCP socket to connect to [address] and [port].
///
/// !arg-type address UtIpv4Address UtIpv6Address UtUnixSocketAddress
/// !return-ref
/// !return-type UtTcpSocket
UtObject *ut_tcp_socket_new(UtObject *address, uint16_t port);

/// Connect the socket. The result of this is given in [callback].
void ut_tcp_socket_connect(UtObject *object, UtObject *callback_object,
                           UtTcpSocketConnectCallback callback);

/// Returns the address this socket is connected to.
///
/// !return-type address UtIpv4Address UtIpv6Address UtUnixSocketAddress
UtObject *ut_tcp_socket_get_address(UtObject *object);

/// Returns the port this socket is connected to on the server.
uint16_t ut_tcp_socket_get_port(UtObject *object);

/// Returns the local port this socket is using.
uint16_t ut_tcp_socket_get_local_port(UtObject *object);

/// Send [data] on this socket.
///
/// !arg-type data UtUint8List
void ut_tcp_socket_send(UtObject *object, UtObject *data);

/// Returns [true] if [object] is a [UtTcpSocket].
bool ut_object_is_tcp_socket(UtObject *object);
