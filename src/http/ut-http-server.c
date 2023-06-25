#include <assert.h>

#include "ut-http-server-client.h"
#include "ut.h"

typedef struct {
  UtObject object;

  // Sockets being listened on.
  UtObject *sockets;

  // Connected clients.
  UtObject *clients;

  // Callback to notify when requests come in.
  UtObject *callback_object;
  UtHttpServerRequestCallback callback;

  // Request in progress.
  UtObject *request;
} UtHttpServer;

static void request_cb(UtObject *object, UtObject *request) {
  UtHttpServer *self = (UtHttpServer *)object;

  ut_object_unref(self->request);
  self->request = ut_object_ref(request);

  if (self->callback_object != NULL && self->callback != NULL) {
    self->callback(self->callback_object, request);
  }
}

static void http_listen_cb(UtObject *object, UtObject *socket) {
  UtHttpServer *self = (UtHttpServer *)object;

  UtObjectRef client = ut_http_server_client_new(socket, object, request_cb);
  ut_list_append(self->clients, client);
  ut_http_server_client_read(client);
}

static void ut_http_server_init(UtObject *object) {
  UtHttpServer *self = (UtHttpServer *)object;
  self->sockets = ut_list_new();
  self->clients = ut_object_list_new();
}

static void ut_http_server_cleanup(UtObject *object) {
  UtHttpServer *self = (UtHttpServer *)object;
  ut_object_unref(self->sockets);
  ut_object_unref(self->clients);
  ut_object_weak_unref(&self->callback_object);
  ut_object_unref(self->request);
}

static UtObjectInterface object_interface = {.type_name = "UtHttpServer",
                                             .init = ut_http_server_init,
                                             .cleanup = ut_http_server_cleanup};

UtObject *ut_http_server_new(UtObject *callback_object,
                             UtHttpServerRequestCallback callback) {
  UtObject *object = ut_object_new(sizeof(UtHttpServer), &object_interface);
  UtHttpServer *self = (UtHttpServer *)object;

  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callback = callback;

  return object;
}

bool ut_http_server_listen_ipv4(UtObject *object, uint16_t port,
                                UtObject **error) {
  assert(ut_object_is_http_server(object));
  UtHttpServer *self = (UtHttpServer *)object;

  UtObjectRef socket = ut_tcp_server_socket_new_ipv4(port);
  ut_list_append(self->sockets, socket);

  return ut_tcp_server_socket_listen(socket, object, http_listen_cb, error);
}

bool ut_http_server_listen_ipv4_any(UtObject *object, uint16_t *port,
                                    UtObject **error) {
  assert(ut_object_is_http_server(object));
  UtHttpServer *self = (UtHttpServer *)object;

  UtObjectRef socket = ut_tcp_server_socket_new_ipv4(0);
  ut_list_append(self->sockets, socket);

  if (!ut_tcp_server_socket_listen(socket, object, http_listen_cb, error)) {
    return false;
  }

  *port = ut_tcp_server_socket_get_port(socket);
  return true;
}

bool ut_http_server_listen_ipv6(UtObject *object, uint16_t port,
                                UtObject **error) {
  assert(ut_object_is_http_server(object));
  UtHttpServer *self = (UtHttpServer *)object;

  UtObjectRef socket = ut_tcp_server_socket_new_ipv6(port);
  ut_list_append(self->sockets, socket);

  return ut_tcp_server_socket_listen(socket, object, http_listen_cb, error);
}

bool ut_http_server_listen_ipv6_any(UtObject *object, uint16_t *port,
                                    UtObject **error) {
  assert(ut_object_is_http_server(object));
  UtHttpServer *self = (UtHttpServer *)object;

  UtObjectRef socket = ut_tcp_server_socket_new_ipv6(0);
  ut_list_append(self->sockets, socket);

  if (!ut_tcp_server_socket_listen(socket, object, http_listen_cb, error)) {
    return false;
  }

  *port = ut_tcp_server_socket_get_port(socket);
  return true;
}

void ut_http_server_respond(UtObject *object, UtObject *request,
                            UtObject *response) {
  assert(ut_object_is_http_server(object));
  UtHttpServer *self = (UtHttpServer *)object;

  // FIXME: Could be multiple requests.
  assert(request == self->request);

  // FIXME: Could be multiple clients.
  UtObject *client = ut_object_list_get_element(self->clients, 0);

  ut_http_server_client_send_response(client, response);
}

bool ut_object_is_http_server(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
