#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "ut-cancel.h"
#include "ut-event-loop.h"
#include "ut-file-descriptor.h"
#include "ut-ipv4-address.h"
#include "ut-ipv6-address.h"
#include "ut-tcp-server-socket.h"
#include "ut-tcp-socket.h"

typedef struct {
  UtObject object;
  sa_family_t family;
  uint16_t port;
  UtObject *fd;
  UtObject *watch_cancel;
  UtTcpServerSocketListenCallback listen_callback;
  void *listen_user_data;
  UtObject *listen_cancel;
} UtTcpServerSocket;

static void ut_tcp_server_socket_cleanup(UtObject *object) {
  UtTcpServerSocket *self = (UtTcpServerSocket *)object;
  ut_object_unref(self->fd);
  if (self->watch_cancel != NULL) {
    ut_cancel_activate(self->watch_cancel);
  }
  ut_object_unref(self->watch_cancel);
  ut_object_unref(self->listen_cancel);
}

static void listen_cb(void *user_data) {
  UtTcpServerSocket *self = user_data;

  // Stop listening for connections when the consumer no longer wants them.
  if (ut_cancel_is_active(self->listen_cancel)) {
    ut_cancel_activate(self->watch_cancel);
    return;
  }

  int fd = accept(ut_file_descriptor_get_fd(self->fd), NULL, NULL);
  assert(fd >= 0);

  UtObjectRef fd_object = ut_file_descriptor_new(fd);
  UtObjectRef child_socket = ut_tcp_socket_new_from_fd(fd_object);
  self->listen_callback(self->listen_user_data, child_socket);
}

static UtObjectInterface object_interface = {.type_name = "UtTcpServerSocket",
                                             .cleanup =
                                                 ut_tcp_server_socket_cleanup,
                                             .interfaces = {{NULL, NULL}}};

static UtObject *socket_new(sa_family_t family, uint16_t port) {
  UtObject *object =
      ut_object_new(sizeof(UtTcpServerSocket), &object_interface);
  UtTcpServerSocket *self = (UtTcpServerSocket *)object;

  self->family = family;
  self->port = port;
  int fd = socket(family, SOCK_STREAM | SOCK_NONBLOCK, 0);
  assert(fd >= 0);
  self->fd = ut_file_descriptor_new(fd);

  return object;
}

UtObject *ut_tcp_server_socket_new_ipv4(uint16_t port) {
  return socket_new(AF_INET, port);
}

UtObject *ut_tcp_server_socket_new_ipv6(uint16_t port) {
  return socket_new(AF_INET6, port);
}

void ut_tcp_server_socket_listen(UtObject *object,
                                 UtTcpServerSocketListenCallback callback,
                                 void *user_data, UtObject *cancel) {
  assert(ut_object_is_tcp_server_socket(object));
  UtTcpServerSocket *self = (UtTcpServerSocket *)object;

  assert(self->listen_callback == NULL);
  assert(callback != NULL);

  self->listen_callback = callback;
  self->listen_user_data = user_data;
  self->listen_cancel = cancel;

  struct sockaddr_in address4;
  struct sockaddr_in6 address6;
  struct sockaddr *address;
  socklen_t address_length;
  if (self->family == AF_INET) {
    memset(&address4, 0, sizeof(address4));
    address4.sin_family = AF_INET;
    address4.sin_port = htons(self->port);
    address = (struct sockaddr *)&address4;
    address_length = sizeof(address4);
  } else {
    memset(&address6, 0, sizeof(address6));
    address6.sin6_family = AF_INET6;
    address6.sin6_port = htons(self->port);
    address = (struct sockaddr *)&address6;
    address_length = sizeof(address6);
  }
  assert(bind(ut_file_descriptor_get_fd(self->fd), address, address_length) ==
         0);

  self->watch_cancel = ut_cancel_new();
  ut_event_loop_add_read_watch(self->fd, listen_cb, self, self->watch_cancel);

  assert(listen(ut_file_descriptor_get_fd(self->fd), 1024) == 0);
}

uint16_t ut_tcp_server_socket_get_port(UtObject *object) {
  assert(ut_object_is_tcp_server_socket(object));
  UtTcpServerSocket *self = (UtTcpServerSocket *)object;
  // Port not know (is either unset or randomly assigned).
  if (self->port == 0) {
    struct sockaddr_in address4;
    struct sockaddr_in6 address6;
    struct sockaddr *address;
    socklen_t address_length;
    if (self->family == AF_INET) {
      address = (struct sockaddr *)&address4;
      address_length = sizeof(address4);
    } else {
      address = (struct sockaddr *)&address6;
      address_length = sizeof(address6);
    }
    assert(getsockname(ut_file_descriptor_get_fd(self->fd), address,
                       &address_length) == 0);
    if (self->family == AF_INET) {
      self->port = ntohs(address4.sin_port);
    } else {
      self->port = ntohs(address6.sin6_port);
    }
  }
  return self->port;
}

bool ut_object_is_tcp_server_socket(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
