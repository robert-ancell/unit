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
#include "ut-input-stream.h"
#include "ut-ipv4-address.h"
#include "ut-ipv6-address.h"
#include "ut-list.h"
#include "ut-output-stream.h"
#include "ut-tcp-socket.h"
#include "ut-uint8-array.h"
#include "ut-uint8-list.h"

typedef struct {
  UtObject object;
  UtObject *address;
  uint16_t port;
  UtObject *fd;
  UtObject *write_watch_cancel;
  UtObject *read_watch_cancel;
  UtTcpSocketConnectCallback connect_callback;
  void *connect_user_data;
  UtObject *connect_cancel;
  UtObject *read_buffer;
  size_t n_read;
  UtInputStreamDataCallback read_callback;
  UtInputStreamClosedCallback read_closed_callback;
  void *read_user_data;
  UtObject *read_cancel;
} UtTcpSocket;

static void ut_tcp_socket_cleanup(UtObject *object) {
  UtTcpSocket *self = (UtTcpSocket *)object;
  ut_object_unref(self->fd);
  if (self->write_watch_cancel != NULL) {
    ut_cancel_activate(self->write_watch_cancel);
  }
  if (self->read_watch_cancel != NULL) {
    ut_cancel_activate(self->read_watch_cancel);
  }
  ut_object_unref(self->write_watch_cancel);
  ut_object_unref(self->connect_cancel);
  ut_object_unref(self->read_buffer);
  ut_object_unref(self->read_watch_cancel);
  ut_object_unref(self->read_cancel);
}

static void connect_cb(void *user_data) {}

static void write_cb(void *user_data) {
  UtTcpSocket *self = user_data;

  // Only one event required to detect connect.
  ut_cancel_activate(self->write_watch_cancel);

  int error; // FIXME: use
  socklen_t error_length = sizeof(error);
  getsockopt(ut_file_descriptor_get_fd(self->fd), SOL_SOCKET, SO_ERROR, &error,
             &error_length);
  assert(error == 0);

  if (!ut_cancel_is_active(self->connect_cancel) &&
      self->connect_callback != NULL) {
    self->connect_callback(self->connect_user_data);
  }
}

static void read_cb(void *user_data) {
  UtTcpSocket *self = user_data;

  // Stop listening for read events when consumer no longer wants them.
  if (ut_cancel_is_active(self->read_cancel)) {
    ut_cancel_activate(self->read_watch_cancel);
    return;
  }

  size_t block_size = 65535;
  if (ut_list_get_length(self->read_buffer) < self->n_read + block_size) {
    ut_list_resize(self->read_buffer, self->n_read + block_size);
  }
  ssize_t n_read = recv(
      ut_file_descriptor_get_fd(self->fd),
      ut_uint8_array_get_data(self->read_buffer) + self->n_read, block_size, 0);
  assert(n_read >= 0);

  self->n_read += n_read;
  UtObjectRef data = ut_list_get_sublist(self->read_buffer, 0, self->n_read);
  size_t n_used = self->read_callback(self->read_user_data, data, n_read > 0);
  assert(n_used <= self->n_read);
  ut_list_remove(self->read_buffer, 0, n_used);
  self->n_read -= n_used;
}

static void set_active(UtTcpSocket *self, bool active) {
  assert(self->read_callback != NULL);

  if (active) {
    if (self->read_watch_cancel == NULL) {
      self->read_watch_cancel = ut_cancel_new();
      ut_event_loop_add_read_watch(self->fd, read_cb, self,
                                   self->read_watch_cancel);
    }
  } else {
    if (self->read_watch_cancel != NULL) {
      ut_cancel_activate(self->read_watch_cancel);
      ut_object_unref(self->read_watch_cancel);
      self->read_watch_cancel = NULL;
    }
  }
}

static void ut_tcp_socket_read(UtObject *object,
                               UtInputStreamDataCallback callback,
                               UtInputStreamClosedCallback closed_callback,
                               void *user_data, UtObject *cancel) {
  UtTcpSocket *self = (UtTcpSocket *)object;

  assert(self->read_callback == NULL);
  assert(callback != NULL);

  self->read_callback = callback;
  self->read_closed_callback = closed_callback;
  self->read_user_data = user_data;
  self->read_cancel = ut_object_ref(cancel);

  self->read_buffer = ut_uint8_array_new();
  set_active(self, true);
}

static void ut_tcp_socket_set_active(UtObject *object, bool active) {
  UtTcpSocket *self = (UtTcpSocket *)object;

  assert(self->read_callback != NULL);

  set_active(self, active);
}

static UtInputStreamInterface input_stream_interface = {
    .read = ut_tcp_socket_read, .set_active = ut_tcp_socket_set_active};

static void ut_tcp_socket_write(UtObject *object, UtObject *data,
                                UtOutputStreamCallback callback,
                                void *user_data, UtObject *cancel) {
  UtTcpSocket *self = (UtTcpSocket *)object;

  ssize_t data_length = ut_list_get_length(data);
  const uint8_t *buffer;
  uint8_t *allocated_buffer = NULL;
  buffer = ut_uint8_list_get_data(data);
  if (buffer == NULL) {
    allocated_buffer = ut_uint8_list_copy_data(data);
    buffer = allocated_buffer;
  }
  assert(send(ut_file_descriptor_get_fd(self->fd), buffer, data_length, 0) ==
         data_length);
  free(allocated_buffer);
}

static UtOutputStreamInterface output_stream_interface = {
    .write = ut_tcp_socket_write};

static UtObjectInterface object_interface = {
    .type_name = "UtTcpSocket",
    .cleanup = ut_tcp_socket_cleanup,
    .interfaces = {{&ut_input_stream_id, &input_stream_interface},
                   {&ut_output_stream_id, &output_stream_interface},
                   {NULL, NULL}}};

UtObject *ut_tcp_socket_new_from_fd(UtObject *fd) {
  UtObject *object = ut_object_new(sizeof(UtTcpSocket), &object_interface);
  UtTcpSocket *self = (UtTcpSocket *)object;

  struct sockaddr address;
  socklen_t address_length = sizeof(address);
  assert(getpeername(ut_file_descriptor_get_fd(fd), &address,
                     &address_length) == 0);
  sa_family_t family = address.sa_family;

  if (family == AF_INET) {
    struct sockaddr_in address4;
    address_length = sizeof(address4);
    assert(getpeername(ut_file_descriptor_get_fd(fd),
                       (struct sockaddr *)&address4, &address_length) == 0);
    self->address = ut_ipv4_address_new(ntohl(address4.sin_addr.s_addr));
    self->port = ntohs(address4.sin_port);
  } else if (family == AF_INET6) {
    struct sockaddr_in6 address6;
    address_length = sizeof(address6);
    assert(getpeername(ut_file_descriptor_get_fd(fd),
                       (struct sockaddr *)&address6, &address_length) == 0);
    self->address = ut_ipv6_address_new(address6.sin6_addr.s6_addr);
    self->port = ntohs(address6.sin6_port);
  } else {
    assert(false);
  }

  self->fd = ut_object_ref(fd);

  return object;
}

UtObject *ut_tcp_socket_new(UtObject *address, uint16_t port) {
  UtObject *object = ut_object_new(sizeof(UtTcpSocket), &object_interface);
  UtTcpSocket *self = (UtTcpSocket *)object;

  assert(address != NULL);
  assert(port > 0);

  self->address = ut_object_ref(address);
  self->port = port;

  sa_family_t family;
  if (ut_object_is_ipv4_address(address)) {
    family = AF_INET;
  } else if (ut_object_is_ipv6_address(address)) {
    family = AF_INET6;
  } else {
    assert(false);
  }

  int fd = socket(family, SOCK_STREAM | SOCK_NONBLOCK, 0);
  assert(fd >= 0);
  self->fd = ut_file_descriptor_new(fd);

  return object;
}

void ut_tcp_socket_connect(UtObject *object,
                           UtTcpSocketConnectCallback callback, void *user_data,
                           UtObject *cancel) {
  assert(ut_object_is_tcp_socket(object));
  UtTcpSocket *self = (UtTcpSocket *)object;

  assert(self->connect_callback == NULL);
  if (callback == NULL) {
    callback = connect_cb;
  }

  self->connect_callback = callback;
  self->connect_user_data = user_data;
  self->connect_cancel = ut_object_ref(cancel);

  self->write_watch_cancel = ut_cancel_new();
  ut_event_loop_add_write_watch(self->fd, write_cb, self,
                                self->write_watch_cancel);

  struct sockaddr_in address4;
  struct sockaddr_in6 address6;
  struct sockaddr *socket_address;
  socklen_t socket_address_length;
  if (ut_object_is_ipv4_address(self->address)) {
    memset(&address4, 0, sizeof(address4));
    address4.sin_family = AF_INET;
    address4.sin_addr.s_addr =
        htonl(ut_ipv4_address_get_address(self->address));
    address4.sin_port = htons(self->port);
    socket_address = (struct sockaddr *)&address4;
    socket_address_length = sizeof(address4);
  } else if (ut_object_is_ipv6_address(self->address)) {
    memset(&address6, 0, sizeof(address6));
    address6.sin6_family = AF_INET6;
    const uint8_t *a6 = ut_ipv6_address_get_address(self->address);
    for (size_t i = 0; i < 16; i++) {
      address6.sin6_addr.s6_addr[i] = a6[i];
    }
    address6.sin6_port = htons(self->port);
    socket_address = (struct sockaddr *)&address6;
    socket_address_length = sizeof(address6);
  } else {
    assert(false);
  }
  int result = connect(ut_file_descriptor_get_fd(self->fd), socket_address,
                       socket_address_length);
  assert(result == 0 || errno == EINPROGRESS);
}

UtObject *ut_tcp_socket_get_address(UtObject *object) {
  assert(ut_object_is_tcp_socket(object));
  UtTcpSocket *self = (UtTcpSocket *)object;
  return self->address;
}

uint16_t ut_tcp_socket_get_port(UtObject *object) {
  assert(ut_object_is_tcp_socket(object));
  UtTcpSocket *self = (UtTcpSocket *)object;
  return self->port;
}

uint16_t ut_tcp_socket_get_local_port(UtObject *object) {
  assert(ut_object_is_tcp_socket(object));
  UtTcpSocket *self = (UtTcpSocket *)object;
  struct sockaddr_in address4;
  struct sockaddr_in6 address6;
  struct sockaddr *socket_address;
  socklen_t socket_address_length;
  if (ut_object_is_ipv4_address(self->address)) {
    socket_address = (struct sockaddr *)&address4;
    socket_address_length = sizeof(address4);
  } else if (ut_object_is_ipv6_address(self->address)) {
    socket_address = (struct sockaddr *)&address6;
    socket_address_length = sizeof(address6);
  } else {
    assert(false);
  }
  assert(getsockname(ut_file_descriptor_get_fd(self->fd), socket_address,
                     &socket_address_length) == 0);
  sa_family_t family = socket_address->sa_family;
  if (family == AF_INET) {
    return ntohs(address4.sin_port);
  } else if (family == AF_INET6) {
    return ntohs(address6.sin6_port);
  } else {
    assert(false);
  }
}

void ut_tcp_socket_send(UtObject *object, UtObject *data) {
  assert(ut_object_is_tcp_socket(object));
  ut_output_stream_write(object, data);
}

bool ut_object_is_tcp_socket(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
