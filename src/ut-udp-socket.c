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
#include "ut-udp-datagram.h"
#include "ut-udp-socket.h"
#include "ut-uint8-array.h"
#include "ut-uint8-list.h"

typedef struct {
  UtObject object;
  sa_family_t family;
  uint16_t port;
  UtObject *fd;
  UtObject *watch_cancel;
  UtInputStreamCallback read_callback;
  void *read_user_data;
  UtObject *read_cancel;
} UtUdpSocket;

static void ut_udp_socket_cleanup(UtObject *object) {
  UtUdpSocket *self = (UtUdpSocket *)object;
  ut_object_unref(self->fd);
  if (self->watch_cancel != NULL) {
    ut_cancel_activate(self->watch_cancel);
  }
  ut_object_unref(self->watch_cancel);
}

static void read_cb(void *user_data) {
  UtUdpSocket *self = user_data;

  // Stop listening for read events when consumer no longer wants them.
  if (ut_cancel_is_active(self->read_cancel)) {
    ut_cancel_activate(self->watch_cancel);
    return;
  }

  UtObjectRef data = ut_uint8_array_new();
  ut_list_resize(data, 65535);

  struct sockaddr_in source_address4;
  struct sockaddr_in6 source_address6;
  struct sockaddr *source_address;
  socklen_t source_address_length;
  if (self->family == AF_INET) {
    source_address = (struct sockaddr *)&source_address4;
    source_address_length = sizeof(source_address4);
  } else {
    source_address = (struct sockaddr *)&source_address6;
    source_address_length = sizeof(source_address6);
  }
  // FIXME: Use MSG_TRUNC to get correct size
  ssize_t n_read = recvfrom(ut_file_descriptor_get_fd(self->fd),
                            ut_uint8_array_get_data(data), 65535, 0,
                            source_address, &source_address_length);
  assert(n_read >= 0);
  ut_list_resize(data, n_read);

  UtObjectRef address = NULL;
  uint16_t port;
  if (self->family == AF_INET) {
    address = ut_ipv4_address_new(ntohl(source_address4.sin_addr.s_addr));
    port = ntohs(source_address4.sin_port);
  } else {
    address = ut_ipv6_address_new(source_address6.sin6_addr.s6_addr);
    port = ntohs(source_address6.sin6_port);
  }

  UtObjectRef datagram = ut_udp_datagram_new(address, port, data);
  size_t n_used = self->read_callback(self->read_user_data, datagram, false);
  // FIXME: Support queuing messages.
  assert(n_used == 1);
}

static void ut_udp_socket_read(UtObject *object, UtInputStreamCallback callback,
                               void *user_data, UtObject *cancel) {
  assert(ut_object_is_udp_socket(object));
  UtUdpSocket *self = (UtUdpSocket *)object;

  assert(self->read_callback == NULL);
  assert(callback != NULL);

  self->read_callback = callback;
  self->read_user_data = user_data;
  self->read_cancel = ut_object_ref(cancel);

  self->watch_cancel = ut_cancel_new();
  ut_event_loop_add_read_watch(self->fd, read_cb, self, self->watch_cancel);
}

static UtInputStreamInterface input_stream_interface = {.read =
                                                            ut_udp_socket_read};

static void ut_udp_socket_write(UtObject *object, UtObject *datagram,
                                UtOutputStreamCallback callback,
                                void *user_data, UtObject *cancel) {
  UtUdpSocket *self = (UtUdpSocket *)object;

  UtObject *address_object = ut_udp_datagram_get_address(datagram);
  uint16_t port = ut_udp_datagram_get_port(datagram);
  struct sockaddr_in address4;
  struct sockaddr_in6 address6;
  struct sockaddr *address;
  socklen_t address_length;
  if (self->family == AF_INET) {
    assert(ut_object_is_ipv4_address(address_object));
    memset(&address4, 0, sizeof(address4));
    address4.sin_family = AF_INET;
    address4.sin_addr.s_addr =
        htonl(ut_ipv4_address_get_address(address_object));
    address4.sin_port = htons(port);
    address = (struct sockaddr *)&address4;
    address_length = sizeof(address4);
  } else {
    assert(ut_object_is_ipv6_address(address_object));
    memset(&address6, 0, sizeof(address6));
    address6.sin6_family = AF_INET6;
    const uint8_t *address_data = ut_ipv6_address_get_address(address_object);
    for (size_t i = 0; i < 16; i++) {
      address6.sin6_addr.s6_addr[i] = address_data[i];
    }
    address6.sin6_port = htons(port);
    address = (struct sockaddr *)&address6;
    address_length = sizeof(address6);
  }

  UtObject *data = ut_udp_datagram_get_data(datagram);
  ssize_t data_length = ut_list_get_length(data);
  const uint8_t *buffer;
  uint8_t *allocated_buffer = NULL;
  buffer = ut_uint8_list_get_data(data);
  if (buffer == NULL) {
    allocated_buffer = ut_uint8_list_copy_data(data);
    buffer = allocated_buffer;
  }
  assert(sendto(ut_file_descriptor_get_fd(self->fd), buffer, data_length, 0,
                address, address_length) == data_length);
  free(allocated_buffer);
}

static UtOutputStreamInterface output_stream_interface = {
    .write = ut_udp_socket_write};

static UtObjectInterface object_interface = {
    .type_name = "UtUdpSocket",
    .cleanup = ut_udp_socket_cleanup,
    .interfaces = {{&ut_input_stream_id, &input_stream_interface},
                   {&ut_output_stream_id, &output_stream_interface},
                   {NULL, NULL}}};

static UtObject *socket_new(sa_family_t family) {
  UtObject *object = ut_object_new(sizeof(UtUdpSocket), &object_interface);
  UtUdpSocket *self = (UtUdpSocket *)object;

  self->family = family;
  int fd = socket(family, SOCK_DGRAM | SOCK_NONBLOCK, 0);
  assert(fd >= 0);
  self->fd = ut_file_descriptor_new(fd);

  return object;
}

UtObject *ut_udp_socket_new_ipv4() { return socket_new(AF_INET); }

UtObject *ut_udp_socket_new_ipv6() { return socket_new(AF_INET6); }

void ut_udp_socket_bind(UtObject *object, uint16_t port) {
  assert(ut_object_is_udp_socket(object));
  UtUdpSocket *self = (UtUdpSocket *)object;

  struct sockaddr_in address4;
  struct sockaddr_in6 address6;
  struct sockaddr *address;
  socklen_t address_length;
  if (self->family == AF_INET) {
    memset(&address4, 0, sizeof(address4));
    address4.sin_family = AF_INET;
    address4.sin_port = htons(port);
    address = (struct sockaddr *)&address4;
    address_length = sizeof(address4);
  } else {
    memset(&address6, 0, sizeof(address6));
    address6.sin6_family = AF_INET6;
    address6.sin6_port = htons(port);
    address = (struct sockaddr *)&address6;
    address_length = sizeof(address6);
  }
  assert(bind(ut_file_descriptor_get_fd(self->fd), address, address_length) ==
         0);

  self->port = port;
}

uint16_t ut_udp_socket_get_port(UtObject *object) {
  assert(ut_object_is_udp_socket(object));
  UtUdpSocket *self = (UtUdpSocket *)object;
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

void ut_udp_socket_send(UtObject *object, UtObject *address, uint16_t port,
                        UtObject *data) {
  assert(ut_object_is_udp_socket(object));
  UtObjectRef datagram = ut_udp_datagram_new(address, port, data);
  ut_output_stream_write(object, datagram);
}

bool ut_object_is_udp_socket(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
