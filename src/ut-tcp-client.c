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
#include "ut-fd-input-stream.h"
#include "ut-fd-output-stream.h"
#include "ut-file-descriptor.h"
#include "ut-input-stream.h"
#include "ut-ip-address-resolver.h"
#include "ut-ipv4-address.h"
#include "ut-list.h"
#include "ut-output-stream.h"
#include "ut-tcp-client.h"

typedef struct {
  UtObject object;
  char *address;
  UtObject *address_resolver;
  uint16_t port;
  UtObject *fd;
  bool connecting;
  bool connected;
  UtTcpClientConnectCallback connect_callback;
  void *connect_user_data;
  UtObject *connect_cancel;
  UtObject *input_stream;
  UtObject *output_stream;
  UtObject *cancel;
} UtTcpClient;

static void connect_cb(void *user_data) {
  UtTcpClient *self = user_data;

  self->connected = true;

  self->input_stream = ut_fd_input_stream_new(self->fd);
  self->output_stream = ut_fd_output_stream_new(self->fd);

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

static void lookup_cb(void *user_data, UtObject *addresses) {
  UtTcpClient *self = user_data;

  UtObjectRef address = ut_list_get_first(addresses);

  int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
  assert(fd >= 0);
  self->fd = ut_file_descriptor_new(fd);

  ut_event_loop_add_write_watch(self->fd, connect_cb, self, self->cancel);

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(ut_ipv4_address_get_address(address));
  addr.sin_port = htons(self->port);
  int connect_result = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
  assert(connect_result == 0 || errno == EINPROGRESS);

  ut_object_unref(self->address_resolver);
  self->address_resolver = NULL;
}

static void disconnect_client(UtTcpClient *self) {
  if (self->fd != NULL) {
    ut_object_unref(self->fd);
    self->fd = NULL;
  }
}

static void ut_tcp_client_init(UtObject *object) {
  UtTcpClient *self = (UtTcpClient *)object;
  self->address = NULL;
  self->address_resolver = NULL;
  self->port = 0;
  self->fd = NULL;
  self->connecting = false;
  self->connected = false;
  self->connect_callback = NULL;
  self->connect_user_data = NULL;
  self->connect_cancel = NULL;
  self->input_stream = NULL;
  self->output_stream = NULL;
  self->cancel = NULL;
}

static void ut_tcp_client_cleanup(UtObject *object) {
  UtTcpClient *self = (UtTcpClient *)object;
  ut_cancel_activate(self->cancel);
  free(self->address);
  ut_object_unref(self->address_resolver);
  ut_object_unref(self->fd);
  ut_object_unref(self->connect_cancel);
  ut_object_unref(self->input_stream);
  ut_object_unref(self->output_stream);
  ut_object_unref(self->cancel);
  disconnect_client(self);
}

static void ut_tcp_client_read(UtObject *object, UtInputStreamCallback callback,
                               void *user_data, UtObject *cancel) {
  assert(ut_object_is_tcp_client(object));
  UtTcpClient *self = (UtTcpClient *)object;
  assert(self->input_stream != NULL);

  ut_input_stream_read(self->input_stream, callback, user_data, cancel);
}

static void ut_tcp_client_set_active(UtObject *object, bool active) {
  assert(ut_object_is_tcp_client(object));
  UtTcpClient *self = (UtTcpClient *)object;
  assert(self->input_stream != NULL);

  ut_input_stream_set_active(self->input_stream, active);
}

static UtInputStreamInterface input_stream_interface = {
    .read = ut_tcp_client_read, .set_active = ut_tcp_client_set_active};

static void ut_tcp_client_write(UtObject *object, UtObject *data,
                                UtOutputStreamCallback callback,
                                void *user_data, UtObject *cancel) {
  UtTcpClient *self = (UtTcpClient *)object;
  assert(self->output_stream != NULL);

  ut_output_stream_write_full(self->output_stream, data, callback, user_data,
                              cancel);
}

static UtOutputStreamInterface output_stream_interface = {
    .write = ut_tcp_client_write};

static UtObjectInterface object_interface = {
    .type_name = "UtTcpClient",
    .init = ut_tcp_client_init,
    .cleanup = ut_tcp_client_cleanup,
    .interfaces = {{&ut_input_stream_id, &input_stream_interface},
                   {&ut_output_stream_id, &output_stream_interface},
                   {NULL, NULL}}};

UtObject *ut_tcp_client_new(const char *address, uint16_t port) {
  UtObject *object = ut_object_new(sizeof(UtTcpClient), &object_interface);
  UtTcpClient *self = (UtTcpClient *)object;
  self->address = strdup(address);
  self->port = port;
  return object;
}

void ut_tcp_client_connect(UtObject *object,
                           UtTcpClientConnectCallback callback, void *user_data,
                           UtObject *cancel) {
  assert(ut_object_is_tcp_client(object));
  UtTcpClient *self = (UtTcpClient *)object;
  assert(!self->connecting);
  self->connecting = true;

  self->connect_callback = callback;
  self->connect_user_data = user_data;
  self->connect_cancel = ut_object_ref(cancel);

  self->address_resolver = ut_ip_address_resolver_new();
  ut_ip_address_resolver_lookup(self->address_resolver, self->address,
                                lookup_cb, self, self->cancel);
}

void ut_tcp_client_disconnect(UtObject *object) {
  assert(ut_object_is_tcp_client(object));
  UtTcpClient *self = (UtTcpClient *)object;
  disconnect_client(self);
}

bool ut_object_is_tcp_client(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
