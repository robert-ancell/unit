#include <assert.h>
#include <stdlib.h>
#include <sys/types.h>

#include "ut-dbus-auth-client.h"
#include "ut-dbus-message-decoder.h"
#include "ut-dbus-message-encoder.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *message;
  UtDBusMethodResponseCallback callback;
  void *user_data;
  UtObject *cancel;
} QueuedMessage;

static void queued_message_cleanup(UtObject *object) {
  QueuedMessage *self = (QueuedMessage *)object;
  ut_object_unref(self->message);
  ut_object_unref(self->cancel);
}

static UtObjectInterface queued_message_object_interface = {
    .type_name = "QueuedMessage",
    .cleanup = queued_message_cleanup,
    .interfaces = {{NULL, NULL}}};

static UtObject *queued_message_new(UtObject *message,
                                    UtDBusMethodResponseCallback callback,
                                    void *user_data, UtObject *cancel) {
  UtObject *object =
      ut_object_new(sizeof(QueuedMessage), &queued_message_object_interface);
  QueuedMessage *self = (QueuedMessage *)object;
  self->message = ut_object_ref(message);
  self->callback = callback;
  self->user_data = user_data;
  self->cancel = ut_object_ref(cancel);
  return object;
}

typedef struct {
  UtObject object;
  UtObject *cancel;
  char *address;
  UtObject *socket;
  UtObject *multiplexer;
  UtObject *auth_input_stream;
  UtObject *message_input_stream;
  UtObject *auth_client;
  UtObject *message_decoder;
  UtObject *message_encoder;
  bool authenticated;
  UtObject *read_cancel;
  size_t last_serial;
  UtObject *message_queue;
  char *unique_name;
  UtDBusMethodRequestCallback method_callback;
  void *method_user_data;
  UtObject *method_cancel;
} UtDBusClient;

static void call_method(UtDBusClient *self, const char *destination,
                        const char *path, const char *interface,
                        const char *name, UtObject *args,
                        UtDBusMethodResponseCallback callback, void *user_data,
                        UtObject *cancel);

static ssize_t find_queued_message(UtDBusClient *self, uint32_t serial) {
  size_t message_queue_length = ut_list_get_length(self->message_queue);
  for (size_t i = 0; i < message_queue_length; i++) {
    QueuedMessage *m =
        (QueuedMessage *)ut_object_list_get_element(self->message_queue, i);
    uint32_t s = ut_dbus_message_get_serial(m->message);
    if (s == serial) {
      return i;
    }
  }

  return -1;
}

static void process_method_call(UtDBusClient *self, UtObject *message) {
  if (self->method_callback != NULL &&
      !ut_cancel_is_active(self->method_cancel)) {
    self->method_callback(self->method_user_data, message);
  }
}

static void process_method_return(UtDBusClient *self, UtObject *message) {
  uint32_t reply_serial = ut_dbus_message_get_reply_serial(message);
  ssize_t i = find_queued_message(self, reply_serial);
  if (i >= 0) {
    QueuedMessage *m =
        (QueuedMessage *)ut_object_list_get_element(self->message_queue, i);
    if (m->callback != NULL) {
      m->callback(m->user_data, ut_dbus_message_get_args(message));
    }
    ut_list_remove(self->message_queue, i, 1);
  }
}

static void process_error(UtDBusClient *self, UtObject *message) {
  uint32_t reply_serial = ut_dbus_message_get_reply_serial(message);
  ssize_t i = find_queued_message(self, reply_serial);
  if (i >= 0) {
    QueuedMessage *m =
        (QueuedMessage *)ut_object_list_get_element(self->message_queue, i);
    if (m->callback != NULL) {
      m->callback(m->user_data,
                  ut_dbus_error_new(ut_dbus_message_get_error_name(message),
                                    ut_dbus_message_get_args(message)));
    }
    ut_list_remove(self->message_queue, i, 1);
  }
}

static void process_message(UtDBusClient *self, UtObject *message) {
  uint8_t type = ut_dbus_message_get_type(message);
  if (type == UT_DBUS_MESSAGE_TYPE_METHOD_CALL) {
    process_method_call(self, message);
  } else if (type == UT_DBUS_MESSAGE_TYPE_METHOD_RETURN) {
    process_method_return(self, message);
  } else if (type == UT_DBUS_MESSAGE_TYPE_ERROR) {
    process_error(self, message);
  }
}

static size_t messages_cb(void *user_data, UtObject *messages, bool complete) {
  UtDBusClient *self = user_data;

  size_t messages_length = ut_list_get_length(messages);
  for (size_t i = 0; i < messages_length; i++) {
    process_message(self, ut_object_list_get_element(messages, i));
  }

  return messages_length;
}

static void auth_complete_cb(void *user_data, const char *guid,
                             bool unix_fd_supported, UtObject *error) {
  UtDBusClient *self = user_data;

  self->authenticated = true;

  self->message_decoder =
      ut_dbus_message_decoder_new(self->message_input_stream);
  ut_input_stream_multiplexer_set_active(self->multiplexer,
                                         self->message_input_stream);
  ut_input_stream_read(self->message_decoder, messages_cb, self,
                       self->read_cancel);

  // Send any queued messages.
  size_t message_queue_length = ut_list_get_length(self->message_queue);
  for (size_t i = 0; i < message_queue_length;) {
    QueuedMessage *m =
        (QueuedMessage *)ut_object_list_get_element(self->message_queue, i);
    UtObjectRef data =
        ut_dbus_message_encoder_encode(self->message_encoder, m->message);
    ut_output_stream_write(self->socket, data);
    if (m->callback == NULL) {
      ut_list_remove(self->message_queue, i, 1);
      message_queue_length--;
    } else {
      i++;
    }
  }
}

static void hello_cb(void *user_data, UtObject *out_args) {
  UtDBusClient *self = user_data;

  self->unique_name = ut_cstring_new(
      ut_string_get_text(ut_object_list_get_element(out_args, 0)));
}

static void connect_cb(void *user_data) {
  UtDBusClient *self = (UtDBusClient *)user_data;

  ut_dbus_auth_client_run(self->auth_client, auth_complete_cb, self,
                          self->cancel);
}

static void connect(UtDBusClient *self) {
  if (self->socket != NULL) {
    return;
  }

  assert(ut_cstring_starts_with(self->address, "unix:path="));
  ut_cstring_ref path = ut_cstring_new_substring(
      self->address, 10, ut_cstring_get_length(self->address));

  UtObjectRef address = ut_unix_socket_address_new(path);
  self->socket = ut_tcp_socket_new(address, 0);

  self->multiplexer = ut_input_stream_multiplexer_new(self->socket);
  self->auth_input_stream = ut_input_stream_multiplexer_add(self->multiplexer);
  self->message_input_stream =
      ut_input_stream_multiplexer_add(self->multiplexer);

  ut_input_stream_multiplexer_set_active(self->multiplexer,
                                         self->auth_input_stream);
  self->auth_client =
      ut_dbus_auth_client_new(self->auth_input_stream, self->socket);

  call_method(self, "org.freedesktop.DBus", "/org/freedesktop/DBus",
              "org.freedesktop.DBus", "Hello", NULL, hello_cb, self,
              self->cancel);

  ut_tcp_socket_connect(self->socket, connect_cb, self, self->cancel);
}

static void send_message(UtDBusClient *self, UtObject *message,
                         UtDBusMethodResponseCallback callback, void *user_data,
                         UtObject *cancel) {
  // Connect if not done yet.
  connect(self);

  // Assign a serial number.
  ut_dbus_message_set_serial(message, self->last_serial + 1);
  self->last_serial++;

  // Queue if expecting response or not yet authenticated.
  if (!self->authenticated || callback != NULL) {
    UtObjectRef queued_message =
        queued_message_new(message, callback, user_data, cancel);
    ut_list_append(self->message_queue, queued_message);
  }

  // Send immediately if authenticated.
  if (self->authenticated) {
    UtObjectRef data =
        ut_dbus_message_encoder_encode(self->message_encoder, message);
    ut_output_stream_write(self->socket, data);
  }
}

static void call_method(UtDBusClient *self, const char *destination,
                        const char *path, const char *interface,
                        const char *name, UtObject *args,
                        UtDBusMethodResponseCallback callback, void *user_data,
                        UtObject *cancel) {
  UtObjectRef message =
      ut_dbus_message_new_method_call(destination, path, interface, name, args);
  send_message(self, message, callback, user_data, cancel);
}

static void ut_dbus_client_init(UtObject *object) {
  UtDBusClient *self = (UtDBusClient *)object;
  self->cancel = ut_cancel_new();
  self->message_encoder = ut_dbus_message_encoder_new();
  self->read_cancel = ut_cancel_new();
  self->message_queue = ut_object_list_new();
}

static void ut_dbus_client_cleanup(UtObject *object) {
  UtDBusClient *self = (UtDBusClient *)object;
  ut_cancel_activate(self->cancel);
  ut_object_unref(self->cancel);
  free(self->address);
  ut_object_unref(self->socket);
  ut_object_unref(self->multiplexer);
  ut_object_unref(self->auth_input_stream);
  ut_object_unref(self->message_input_stream);
  ut_object_unref(self->auth_client);
  ut_object_unref(self->message_decoder);
  ut_object_unref(self->message_encoder);
  ut_cancel_activate(self->read_cancel);
  ut_object_unref(self->read_cancel);
  ut_object_unref(self->message_queue);
  free(self->unique_name);
  ut_cancel_activate(self->method_cancel);
}

static UtObjectInterface object_interface = {.type_name = "UtDBusClient",
                                             .init = ut_dbus_client_init,
                                             .cleanup = ut_dbus_client_cleanup,
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_dbus_client_new(const char *address) {
  UtObject *object = ut_object_new(sizeof(UtDBusClient), &object_interface);
  UtDBusClient *self = (UtDBusClient *)object;
  self->address = ut_cstring_new(address);
  return object;
}

UtObject *ut_dbus_client_new_system() {
  const char *address = getenv("DBUS_SYSTEM_BUS_ADDRESS");
  if (address == NULL) {
    address = "unix:path=/var/run/dbus/system_bus_socket";
  }
  return ut_dbus_client_new(address);
}

UtObject *ut_dbus_client_new_session() {
  const char *address = getenv("DBUS_SESSION_BUS_ADDRESS");
  assert(address != NULL);
  return ut_dbus_client_new(address);
}

void ut_dbus_client_set_method_call_handler(
    UtObject *object, UtDBusMethodRequestCallback callback, void *user_data,
    UtObject *cancel) {
  assert(ut_object_is_dbus_client(object));
  UtDBusClient *self = (UtDBusClient *)object;
  assert(self->method_callback == NULL);

  self->method_callback = callback;
  self->method_user_data = user_data;
  self->method_cancel = ut_object_ref(cancel);
}

const char *ut_dbus_client_get_unique_name(UtObject *object) {
  assert(ut_object_is_dbus_client(object));
  UtDBusClient *self = (UtDBusClient *)object;
  return self->unique_name;
}

void ut_dbus_client_call_method(UtObject *object, const char *destination,
                                const char *path, const char *interface,
                                const char *name, UtObject *args,
                                UtDBusMethodResponseCallback callback,
                                void *user_data, UtObject *cancel) {
  assert(ut_object_is_dbus_client(object));
  UtDBusClient *self = (UtDBusClient *)object;
  call_method(self, destination, path, interface, name, args, callback,
              user_data, cancel);
}

void ut_dbus_client_send_reply(UtObject *object, UtObject *method_call,
                               UtObject *args) {
  assert(ut_object_is_dbus_client(object));
  UtDBusClient *self = (UtDBusClient *)object;
  UtObjectRef message = ut_dbus_message_new_method_return(
      ut_dbus_message_get_serial(method_call), args);
  ut_dbus_message_set_sender(message,
                             ut_dbus_message_get_destination(method_call));
  ut_dbus_message_set_destination(message,
                                  ut_dbus_message_get_sender(method_call));
  send_message(self, message, NULL, NULL, NULL);
}

bool ut_object_is_dbus_client(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
