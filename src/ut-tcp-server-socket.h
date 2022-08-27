#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef void (*UtTcpServerSocketListenCallback)(void *user_data,
                                                UtObject *socket);

UtObject *ut_tcp_server_socket_new_ipv4(uint16_t port);

UtObject *ut_tcp_server_socket_new_ipv6(uint16_t port);

UtObject *ut_tcp_server_socket_new_unix(const char *path);

void ut_tcp_server_socket_listen(UtObject *object,
                                 UtTcpServerSocketListenCallback callback,
                                 void *user_data, UtObject *cancel);

uint16_t ut_tcp_server_socket_get_port(UtObject *object);

bool ut_object_is_tcp_server_socket(UtObject *object);
