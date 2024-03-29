#include <assert.h>
#include <string.h>
#include <unistd.h>

#include "ut-dbus-auth-client.h"
#include "ut.h"

typedef enum {
  AUTH_STATE_IDLE,
  AUTH_STATE_PROBE_MECHANISMS,
  AUTH_STATE_AUTH_EXTERNAL,
  AUTH_STATE_NEGOTIATE_UNIX_FD,
  AUTH_STATE_DONE
} AuthState;

typedef struct {
  UtObject object;
  UtObject *input_stream;
  UtObject *output_stream;
  bool negotiate_unix_fd;
  AuthState state;
  char *guid;
  bool unix_fd_supported;
  UtObject *error;
  UtObject *complete_callback_object;
  UtAuthCompleteCallback complete_callback;
} UtDBusAuthClient;

static void send_auth_message(UtDBusAuthClient *self, const char *message) {
  UtObjectRef text = ut_string_new(message);
  ut_string_append(text, "\r\n");
  UtObjectRef utf8 = ut_string_get_utf8(text);
  ut_output_stream_write(self->output_stream, utf8);
}

static void send_auth_start(UtDBusAuthClient *self) {
  send_auth_message(self, "AUTH");
}

static void send_auth_external(UtDBusAuthClient *self) {
  UtObjectRef message = ut_string_new("AUTH EXTERNAL ");
  ut_cstring_ref auth_id = ut_cstring_new_printf("%d", (int)getuid());
  for (char *c = auth_id; *c != '\0'; c++) {
    ut_cstring_ref hex = ut_cstring_new_printf("%02x", *c);
    ut_string_append(message, hex);
  }
  send_auth_message(self, ut_string_get_text(message));
}

static void send_negotiate_unix_fd(UtDBusAuthClient *self) {
  send_auth_message(self, "NEGOTIATE_UNIX_FD");
}

static void send_auth_end(UtDBusAuthClient *self) {
  send_auth_message(self, "BEGIN");
}

static void done(UtDBusAuthClient *self) {
  self->state = AUTH_STATE_DONE;

  ut_input_stream_close(self->input_stream);

  if (self->complete_callback_object != NULL) {
    self->complete_callback(self->complete_callback_object, self->guid,
                            self->unix_fd_supported,
                            ut_object_ref(self->error));
  }
}

static void process_line(UtDBusAuthClient *self, const char *line) {
  char *command_end = strchr(line, ' ');
  ut_cstring_ref command = NULL;
  ut_cstring_ref args = NULL;
  if (command_end == NULL) {
    command = ut_cstring_new(line);
    args = ut_cstring_new("");
  } else {
    command = ut_cstring_new_sized(line, command_end - line);
    args = ut_cstring_new(command_end + 1);
  }

  if (ut_cstring_equal(command, "REJECTED")) {
    if (self->state == AUTH_STATE_PROBE_MECHANISMS) {
      self->state = AUTH_STATE_AUTH_EXTERNAL;
      send_auth_external(self);
    } else {
      self->error = ut_error_new("No supported auth mechanism");
      done(self);
    }
  } else if (ut_cstring_equal(command, "OK")) {
    assert(self->state == AUTH_STATE_AUTH_EXTERNAL);
    free(self->guid);
    self->guid = ut_cstring_new(args);
    if (self->negotiate_unix_fd) {
      self->state = AUTH_STATE_NEGOTIATE_UNIX_FD;
      send_negotiate_unix_fd(self);
    } else {
      send_auth_end(self);
      done(self);
    }
  } else if (ut_cstring_equal(command, "AGREE_UNIX_FD")) {
    assert(self->state == AUTH_STATE_NEGOTIATE_UNIX_FD);
    self->unix_fd_supported = true;
    send_auth_end(self);
    done(self);
  } else if (ut_cstring_equal(command, "ERROR")) {
    if (self->state == AUTH_STATE_NEGOTIATE_UNIX_FD) {
      send_auth_end(self);
      done(self);
    } else {
      self->error = ut_error_new_take(
          ut_cstring_new_printf("Error during authentication: %s", args));
      done(self);
    }
  } else {
    assert(false);
  }
}

static char *read_line(UtObject *data, size_t *offset) {
  size_t length = ut_list_get_length(data);

  for (size_t end = *offset; end < length - 1; end++) {
    if (ut_uint8_list_get_element(data, end) == '\r' &&
        ut_uint8_list_get_element(data, end + 1) == '\n') {
      size_t length = end - *offset;
      char *line = malloc(sizeof(char) * (length + 1));
      for (size_t i = 0; i < length; i++) {
        line[i] = ut_uint8_list_get_element(data, *offset + i);
      }
      line[length] = '\0';
      *offset = end + 2;
      return line;
    }
  }

  return NULL;
}

static size_t read_cb(UtObject *object, UtObject *data, bool complete) {
  UtDBusAuthClient *self = (UtDBusAuthClient *)object;

  size_t offset = 0;
  while (self->state != AUTH_STATE_DONE) {
    ut_cstring_ref line = read_line(data, &offset);
    if (line == NULL) {
      break;
    }

    process_line(self, line);
  }

  return offset;
}

static void ut_dbus_auth_client_init(UtObject *object) {
  UtDBusAuthClient *self = (UtDBusAuthClient *)object;
  self->state = AUTH_STATE_IDLE;
}

static void ut_dbus_auth_client_cleanup(UtObject *object) {
  UtDBusAuthClient *self = (UtDBusAuthClient *)object;

  ut_input_stream_close(self->input_stream);

  ut_object_unref(self->input_stream);
  ut_object_unref(self->output_stream);
  free(self->guid);
  ut_object_unref(self->error);
  ut_object_weak_unref(&self->complete_callback_object);
}

static UtObjectInterface object_interface = {.type_name = "UtDBusAuthClient",
                                             .init = ut_dbus_auth_client_init,
                                             .cleanup =
                                                 ut_dbus_auth_client_cleanup};

UtObject *ut_dbus_auth_client_new(UtObject *input_stream,
                                  UtObject *output_stream) {
  UtObject *object = ut_object_new(sizeof(UtDBusAuthClient), &object_interface);
  UtDBusAuthClient *self = (UtDBusAuthClient *)object;
  self->input_stream = ut_object_ref(input_stream);
  self->output_stream = ut_object_ref(output_stream);
  return object;
}

void ut_dbus_auth_client_set_negotiate_unix_fd(UtObject *object,
                                               bool negotiate_unix_fd) {
  assert(ut_object_is_dbus_auth_client(object));
  UtDBusAuthClient *self = (UtDBusAuthClient *)object;
  self->negotiate_unix_fd = negotiate_unix_fd;
}

void ut_dbus_auth_client_run(UtObject *object, UtObject *callback_object,
                             UtAuthCompleteCallback callback) {
  assert(ut_object_is_dbus_auth_client(object));
  UtDBusAuthClient *self = (UtDBusAuthClient *)object;

  assert(self->state == AUTH_STATE_IDLE);
  self->state = AUTH_STATE_PROBE_MECHANISMS;

  assert(callback != NULL);
  ut_object_weak_ref(callback_object, &self->complete_callback_object);
  self->complete_callback = callback;

  ut_input_stream_read(self->input_stream, object, read_cb);

  // Send empty byte, which was used for sending credentials (no longer
  // required).
  UtObjectRef start = ut_uint8_list_new_from_elements(1, 0);
  ut_output_stream_write(self->output_stream, start);

  send_auth_start(self);
}

bool ut_object_is_dbus_auth_client(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
