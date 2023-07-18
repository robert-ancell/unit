#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef void (*UtTcpSocketConnectCallback)(UtObject *object, UtObject *error);

UtObject *ut_tcp_socket_new_from_fd(UtObject *fd);

UtObject *ut_tcp_socket_new(UtObject *address, uint16_t port);

void ut_tcp_socket_connect(UtObject *object, UtObject *callback_object,
                           UtTcpSocketConnectCallback callback);

UtObject *ut_tcp_socket_get_address(UtObject *object);

uint16_t ut_tcp_socket_get_port(UtObject *object);

uint16_t ut_tcp_socket_get_local_port(UtObject *object);

void ut_tcp_socket_send(UtObject *object, UtObject *data);

bool ut_object_is_tcp_socket(UtObject *object);
