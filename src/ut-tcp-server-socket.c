#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "ut.h"

typedef struct {
  UtObject object;
  sa_family_t family;
  char *unix_path;
  uint16_t port;
  UtObject *fd;
  UtObject *watch;
  UtObject *listen_callback_object;
  UtTcpServerSocketListenCallback listen_callback;
} UtTcpServerSocket;

static void ut_tcp_server_socket_cleanup(UtObject *object) {
  UtTcpServerSocket *self = (UtTcpServerSocket *)object;
  free(self->unix_path);
  ut_object_unref(self->fd);
  ut_object_unref(self->watch);
  ut_object_weak_unref(&self->listen_callback_object);
}

static void listen_cb(UtObject *object) {
  UtTcpServerSocket *self = (UtTcpServerSocket *)object;

  int fd = accept(ut_file_descriptor_get_fd(self->fd), NULL, NULL);
  assert(fd >= 0);

  UtObjectRef fd_object = ut_file_descriptor_new(fd);
  UtObjectRef child_socket = ut_tcp_socket_new_from_fd(fd_object);
  if (self->listen_callback_object != NULL) {
    self->listen_callback(self->listen_callback_object, child_socket);
  }
}

static UtObjectInterface object_interface = {
    .type_name = "UtTcpServerSocket", .cleanup = ut_tcp_server_socket_cleanup};

static UtObject *socket_new(sa_family_t family, const char *path,
                            uint16_t port) {
  UtObject *object =
      ut_object_new(sizeof(UtTcpServerSocket), &object_interface);
  UtTcpServerSocket *self = (UtTcpServerSocket *)object;

  self->family = family;
  self->unix_path = path != NULL ? ut_cstring_new(path) : NULL;
  self->port = port;
  int fd = socket(family, SOCK_STREAM | SOCK_NONBLOCK, 0);
  assert(fd >= 0);
  self->fd = ut_file_descriptor_new(fd);

  return object;
}

UtObject *ut_tcp_server_socket_new_ipv4(uint16_t port) {
  return socket_new(AF_INET, NULL, port);
}

UtObject *ut_tcp_server_socket_new_ipv6(uint16_t port) {
  return socket_new(AF_INET6, NULL, port);
}

UtObject *ut_tcp_server_socket_new_unix(const char *path) {
  return socket_new(AF_UNIX, path, 0);
}

bool ut_tcp_server_socket_listen(UtObject *object, UtObject *callback_object,
                                 UtTcpServerSocketListenCallback callback,
                                 UtObject **error) {
  assert(ut_object_is_tcp_server_socket(object));
  UtTcpServerSocket *self = (UtTcpServerSocket *)object;

  assert(self->listen_callback == NULL);
  assert(callback != NULL);

  ut_object_weak_ref(callback_object, &self->listen_callback_object);
  self->listen_callback = callback;

  struct sockaddr_in address4;
  struct sockaddr_in6 address6;
  struct sockaddr_un address_unix;
  struct sockaddr *address;
  socklen_t address_length;
  if (self->family == AF_INET) {
    memset(&address4, 0, sizeof(address4));
    address4.sin_family = AF_INET;
    address4.sin_port = htons(self->port);
    address = (struct sockaddr *)&address4;
    address_length = sizeof(address4);
  } else if (self->family == AF_INET6) {
    memset(&address6, 0, sizeof(address6));
    address6.sin6_family = AF_INET6;
    address6.sin6_port = htons(self->port);
    address = (struct sockaddr *)&address6;
    address_length = sizeof(address6);
  } else if (self->family == AF_UNIX) {
    memset(&address_unix, 0, sizeof(address_unix));
    address_unix.sun_family = AF_UNIX;
    snprintf(address_unix.sun_path, sizeof(address_unix.sun_path), "%s",
             self->unix_path);
    address = (struct sockaddr *)&address_unix;
    address_length = sizeof(address_unix);
  } else {
    assert(false);
  }
  if (bind(ut_file_descriptor_get_fd(self->fd), address, address_length) != 0) {
    if (error != NULL) {
      *error = ut_system_error_new(errno);
    }
    return false;
  }

  self->watch = ut_event_loop_add_read_watch(self->fd, object, listen_cb);

  if (listen(ut_file_descriptor_get_fd(self->fd), 1024) != 0) {
    if (error != NULL) {
      *error = ut_system_error_new(errno);
    }
    return false;
  }

  return true;
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
    } else if (self->family == AF_INET6) {
      address = (struct sockaddr *)&address6;
      address_length = sizeof(address6);
    } else if (self->family == AF_UNIX) {
      return 0;
    } else {
      assert(false);
    }
    assert(getsockname(ut_file_descriptor_get_fd(self->fd), address,
                       &address_length) == 0);
    if (self->family == AF_INET) {
      self->port = ntohs(address4.sin_port);
    } else if (self->family == AF_INET6) {
      self->port = ntohs(address6.sin6_port);
    } else {
      assert(false);
    }
  }
  return self->port;
}

bool ut_object_is_tcp_server_socket(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
