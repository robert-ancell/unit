#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef void (*UtTcpServerSocketListenCallback)(UtObject *object,
                                                UtObject *socket);

UtObject *ut_tcp_server_socket_new_ipv4(uint16_t port);

UtObject *ut_tcp_server_socket_new_ipv6(uint16_t port);

UtObject *ut_tcp_server_socket_new_unix(const char *path);

bool ut_tcp_server_socket_listen(UtObject *object, UtObject *callback_object,
                                 UtTcpServerSocketListenCallback callback,
                                 UtObject **error);

uint16_t ut_tcp_server_socket_get_port(UtObject *object);

bool ut_object_is_tcp_server_socket(UtObject *object);
