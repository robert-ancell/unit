#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Method called when a remote client connects via [socket].
///
/// !arg-type socket UtTcpSocket
typedef void (*UtTcpServerSocketListenCallback)(UtObject *object,
                                                UtObject *socket);

/// Creates a new TCP server socket that listens on the provided IPv4 [port].
///
/// !return-ref
/// !return-type UtTcpServerSocket
UtObject *ut_tcp_server_socket_new_ipv4(uint16_t port);

/// Creates a new TCP server socket that listens on the provided IPv6 [port].
///
/// !return-ref
/// !return-type UtTcpServerSocket
UtObject *ut_tcp_server_socket_new_ipv6(uint16_t port);

/// Creates a new TCP server socket that listens on the Unix domain socket
/// [path].
///
/// !return-ref
/// !return-type UtTcpServerSocket
UtObject *ut_tcp_server_socket_new_unix(const char *path);

/// Listen for incoming connections. Each new connection is reported in
/// [callback]. If fails returns false and sets [error].
bool ut_tcp_server_socket_listen(UtObject *object, UtObject *callback_object,
                                 UtTcpServerSocketListenCallback callback,
                                 UtObject **error);

/// Returns the port this socket is listening on.
uint16_t ut_tcp_server_socket_get_port(UtObject *object);

/// Returns [true] if [object] is a [UtTcpServerSocket].
bool ut_object_is_tcp_server_socket(UtObject *object);
