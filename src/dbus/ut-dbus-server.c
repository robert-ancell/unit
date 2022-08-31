#include <assert.h>
#include <string.h>
#include <sys/types.h>

#include "ut-dbus-auth-server.h"
#include "ut-dbus-message-decoder.h"
#include "ut-dbus-message-encoder.h"
#include "ut.h"

typedef struct _UtDBusServer UtDBusServer;

typedef struct {
  UtObject object;
  UtDBusServer *server;
  UtObject *socket;
  UtObject *multiplexer;
  UtObject *auth_input_stream;
  UtObject *message_input_stream;
  UtObject *auth_server;
  UtObject *message_decoder;
  // FIXME bool received_hello;
  UtObject *names;
  UtObject *cancel;
} UtDBusServerClient;

static void ut_dbus_server_client_init(UtObject *object) {
  UtDBusServerClient *self = (UtDBusServerClient *)object;
  self->names = ut_string_list_new();
  self->cancel = ut_cancel_new();
}

static void ut_dbus_server_client_cleanup(UtObject *object) {
  UtDBusServerClient *self = (UtDBusServerClient *)object;
  ut_cancel_activate(self->cancel);
  ut_object_unref(self->socket);
  ut_object_unref(self->multiplexer);
  ut_object_unref(self->auth_input_stream);
  ut_object_unref(self->message_input_stream);
  ut_object_unref(self->auth_server);
  ut_object_unref(self->message_decoder);
  ut_object_unref(self->names);
  ut_object_unref(self->cancel);
}

static UtObjectInterface client_object_interface = {
    .type_name = "UtDBusServerClient",
    .init = ut_dbus_server_client_init,
    .cleanup = ut_dbus_server_client_cleanup,
    .interfaces = {{NULL, NULL}}};

UtObject *ut_dbus_server_client_new(UtDBusServer *server, UtObject *socket,
                                    const char *unique_name) {
  UtObject *object =
      ut_object_new(sizeof(UtDBusServerClient), &client_object_interface);
  UtDBusServerClient *self = (UtDBusServerClient *)object;
  self->server = server;
  self->socket = ut_object_ref(socket);
  ut_string_list_append(self->names, unique_name);
  return object;
}

struct _UtDBusServer {
  UtObject object;
  UtObject *sockets;
  UtObject *clients;
  int next_client_index;
  int next_message_serial;
  UtObject *cancel;
};

static const char *get_unique_name(UtDBusServerClient *self) {
  return ut_string_list_get_element(self->names, 0);
}

static bool client_has_name(UtDBusServerClient *self, const char *name) {
  size_t n_names = ut_list_get_length(self->names);
  for (size_t i = 0; i < n_names; i++) {
    if (ut_cstring_equal(ut_string_list_get_element(self->names, i), name)) {
      return true;
    }
  }

  return false;
}

static void send_message(UtDBusServer *self, UtObject *message) {
  const char *destination = ut_dbus_message_get_destination(message);
  if (destination == NULL) {
    return;
  }

  size_t n_clients = ut_list_get_length(self->clients);
  UtDBusServerClient *target_client = NULL;
  for (size_t i = 0; i < n_clients; i++) {
    UtObject *client = ut_object_list_get_element(self->clients, i);
    UtDBusServerClient *c = (UtDBusServerClient *)client;
    if (client_has_name(c, destination)) {
      target_client = c;
      break;
    }
  }
  if (target_client == NULL) {
    return;
  }

  UtObjectRef encoder = ut_dbus_message_encoder_new();
  UtObjectRef data = ut_dbus_message_encoder_encode(encoder, message);
  ut_output_stream_write(target_client->socket, data);
}

static void send_reply(UtDBusServer *self, UtObject *method_call,
                       UtObject *args) {
  uint32_t serial = ut_dbus_message_get_serial(method_call);
  UtObjectRef message = ut_dbus_message_new_method_return(serial, args);
  ut_dbus_message_set_sender(message, "org.freedesktop.DBus");
  ut_dbus_message_set_destination(message,
                                  ut_dbus_message_get_sender(method_call));
  ut_dbus_message_set_serial(message, self->next_message_serial);
  self->next_message_serial++;
  send_message(self, message);
}

static void process_server_method_call(UtDBusServerClient *self,
                                       UtObject *message) {
  const char *interface = ut_dbus_message_get_interface(message);
  const char *method_name = ut_dbus_message_get_member(message);

  if (interface == NULL) {
    interface = "";
  }

  if (ut_cstring_equal(interface, "org.freedesktop.DBus")) {
    if (ut_cstring_equal(method_name, "Hello")) {
      const char *unique_name = get_unique_name(self);
      UtObjectRef args =
          ut_list_new_from_elements_take(ut_string_new(unique_name), NULL);
      uint32_t serial = ut_dbus_message_get_serial(message);
      UtObjectRef message = ut_dbus_message_new_method_return(serial, args);
      ut_dbus_message_set_sender(message, "org.freedesktop.DBus");
      ut_dbus_message_set_destination(message, unique_name);
      ut_dbus_message_set_serial(message, self->server->next_message_serial);
      self->server->next_message_serial++;
      send_message(self->server, message);
    } else if (ut_cstring_equal(method_name, "Ping")) {
      send_reply(self->server, message, NULL);
    } else {
    }
  } else {
  }
}

static void process_server_message(UtDBusServerClient *self,
                                   UtObject *message) {
  uint8_t type = ut_dbus_message_get_type(message);

  if (type == UT_DBUS_MESSAGE_TYPE_METHOD_CALL) {
    process_server_method_call(self, message);
  }
}

static void process_message(UtDBusServerClient *self, UtObject *message) {
  if (ut_dbus_message_get_sender(message) == NULL) {
    const char *unique_name = get_unique_name(self);
    ut_dbus_message_set_sender(message, unique_name);
  }

  const char *destination = ut_dbus_message_get_destination(message);
  if (ut_cstring_equal(destination, "org.freedesktop.DBus")) {
    process_server_message(self, message);
  } else {
    send_message(self->server, message);
  }
}

static size_t messages_cb(void *user_data, UtObject *messages, bool complete) {
  UtDBusServerClient *self = user_data;

  size_t messages_length = ut_list_get_length(messages);
  for (size_t i = 0; i < messages_length; i++) {
    UtObjectRef message = ut_list_get_element(messages, i);
    process_message(self, message);
  }

  return messages_length;
}

static void auth_complete_cb(void *user_data, UtObject *error) {
  UtDBusServerClient *self = user_data;

  self->message_decoder =
      ut_dbus_message_decoder_new(self->message_input_stream);
  ut_input_stream_multiplexer_set_active(self->multiplexer,
                                         self->message_input_stream);
  ut_input_stream_read(self->message_decoder, messages_cb, self, self->cancel);
}

static void listen_cb(void *user_data, UtObject *socket) {
  UtDBusServer *self = user_data;

  ut_cstring_ref unique_name =
      ut_cstring_new_printf(":1.%d", self->next_client_index);
  self->next_client_index++;

  UtObjectRef client_object =
      ut_dbus_server_client_new(self, socket, unique_name);
  UtDBusServerClient *client = (UtDBusServerClient *)client_object;
  ut_list_append(self->clients, client_object);

  client->multiplexer = ut_input_stream_multiplexer_new(client->socket);
  client->auth_input_stream =
      ut_input_stream_multiplexer_add(client->multiplexer);
  client->message_input_stream =
      ut_input_stream_multiplexer_add(client->multiplexer);

  ut_input_stream_multiplexer_set_active(client->multiplexer,
                                         client->auth_input_stream);
  client->auth_server =
      ut_dbus_auth_server_new(client->auth_input_stream, client->socket);

  ut_dbus_auth_server_run(client->auth_server, auth_complete_cb, client,
                          client->cancel);
}

static void ut_dbus_server_init(UtObject *object) {
  UtDBusServer *self = (UtDBusServer *)object;
  self->sockets = ut_list_new();
  self->clients = ut_object_list_new();
  self->next_message_serial = 1;
  self->cancel = ut_cancel_new();
}

static void ut_dbus_server_cleanup(UtObject *object) {
  UtDBusServer *self = (UtDBusServer *)object;
  ut_cancel_activate(self->cancel);
  ut_object_unref(self->sockets);
  ut_object_unref(self->clients);
  ut_object_unref(self->cancel);
}

static UtObjectInterface object_interface = {.type_name = "UtDBusServer",
                                             .init = ut_dbus_server_init,
                                             .cleanup = ut_dbus_server_cleanup,
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_dbus_server_new() {
  return ut_object_new(sizeof(UtDBusServer), &object_interface);
}

void ut_dbus_server_listen_unix(UtObject *object, const char *path) {
  assert(ut_object_is_dbus_server(object));
  UtDBusServer *self = (UtDBusServer *)object;
  UtObjectRef socket = ut_tcp_server_socket_new_unix(path);
  ut_list_append(self->sockets, socket);
  ut_tcp_server_socket_listen(socket, listen_cb, self, self->cancel);
}

bool ut_object_is_dbus_server(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
