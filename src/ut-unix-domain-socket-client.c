#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "ut-cancel.h"
#include "ut-event-loop.h"
#include "ut-fd-input-stream.h"
#include "ut-fd-output-stream.h"
#include "ut-file-descriptor.h"
#include "ut-input-stream.h"
#include "ut-output-stream.h"
#include "ut-unix-domain-socket-client.h"

typedef struct {
  UtObject object;
  char *path;
  UtObject *fd;
  UtObject *input_stream;
  UtObject *output_stream;
} UtUnixDomainSocketClient;

static void disconnect_client(UtUnixDomainSocketClient *self) {
  if (self->fd != NULL) {
    ut_object_unref(self->fd);
    self->fd = NULL;
  }
}

static void ut_unix_domain_socket_client_cleanup(UtObject *object) {
  UtUnixDomainSocketClient *self = (UtUnixDomainSocketClient *)object;
  free(self->path);
  ut_object_unref(self->fd);
  ut_object_unref(self->input_stream);
  ut_object_unref(self->output_stream);
  disconnect_client(self);
}

static void
ut_unix_domain_socket_client_read(UtObject *object,
                                  UtInputStreamDataCallback callback,
                                  UtInputStreamClosedCallback closed_callback,
                                  void *user_data, UtObject *cancel) {
  assert(ut_object_is_unix_domain_socket_client(object));
  UtUnixDomainSocketClient *self = (UtUnixDomainSocketClient *)object;
  assert(self->input_stream != NULL);

  ut_input_stream_read(self->input_stream, callback, closed_callback, user_data,
                       cancel);
}

static UtInputStreamInterface input_stream_interface = {
    .read = ut_unix_domain_socket_client_read};

static void ut_unix_domain_socket_client_write(UtObject *object, UtObject *data,
                                               UtOutputStreamCallback callback,
                                               void *user_data,
                                               UtObject *cancel) {
  UtUnixDomainSocketClient *self = (UtUnixDomainSocketClient *)object;
  assert(self->output_stream != NULL);

  ut_output_stream_write_full(self->output_stream, data, callback, user_data,
                              cancel);
}

static UtOutputStreamInterface output_stream_interface = {
    .write = ut_unix_domain_socket_client_write};

static UtObjectInterface object_interface = {
    .type_name = "UtUnixDomainSocketClient",
    .cleanup = ut_unix_domain_socket_client_cleanup,
    .interfaces = {{&ut_input_stream_id, &input_stream_interface},
                   {&ut_output_stream_id, &output_stream_interface},
                   {NULL, NULL}}};

UtObject *ut_unix_domain_socket_client_new(const char *path) {
  UtObject *object =
      ut_object_new(sizeof(UtUnixDomainSocketClient), &object_interface);
  UtUnixDomainSocketClient *self = (UtUnixDomainSocketClient *)object;
  self->path = strdup(path);
  return object;
}

void ut_unix_domain_socket_client_connect(UtObject *object) {
  assert(ut_object_is_unix_domain_socket_client(object));
  UtUnixDomainSocketClient *self = (UtUnixDomainSocketClient *)object;
  assert(self->fd == NULL);

  int fd = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
  assert(fd >= 0);
  self->fd = ut_file_descriptor_new(fd);

  struct sockaddr_un address;
  address.sun_family = AF_UNIX;
  snprintf(address.sun_path, sizeof(address.sun_path), "%s", self->path);
  assert(connect(fd, (const struct sockaddr *)&address, sizeof(address)) == 0);

  self->input_stream = ut_fd_input_stream_new(self->fd);
  ut_fd_input_stream_set_receive_fds(self->input_stream, true);
  self->output_stream = ut_fd_output_stream_new(self->fd);
}

void ut_unix_domain_socket_client_disconnect(UtObject *object) {
  assert(ut_object_is_unix_domain_socket_client(object));
  UtUnixDomainSocketClient *self = (UtUnixDomainSocketClient *)object;
  disconnect_client(self);
}

bool ut_object_is_unix_domain_socket_client(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
