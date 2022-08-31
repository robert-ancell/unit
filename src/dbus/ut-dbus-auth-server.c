#include <assert.h>
#include <string.h>
#include <unistd.h>

#include "ut-dbus-auth-server.h"
#include "ut.h"

typedef enum {
  AUTH_STATE_IDLE,
  AUTH_STATE_CREDENTIALS,
  AUTH_STATE_READY,
  AUTH_STATE_DONE
} AuthState;

typedef struct {
  UtObject object;
  UtObject *input_stream;
  UtObject *output_stream;
  UtObject *read_cancel;
  AuthState state;
  bool authenticated;
  UtObject *error;
  UtAuthCompleteCallback complete_callback;
  void *complete_user_data;
  UtObject *complete_cancel;
} UtDBusAuthServer;

static void send_auth_message(UtDBusAuthServer *self, const char *message) {
  UtObjectRef text = ut_string_new(message);
  ut_string_append(text, "\r\n");
  UtObjectRef utf8 = ut_string_get_utf8(text);
  ut_output_stream_write(self->output_stream, utf8);
}

static void done(UtDBusAuthServer *self) {
  self->state = AUTH_STATE_DONE;
  ut_cancel_activate(self->read_cancel);

  if (!ut_cancel_is_active(self->complete_cancel)) {
    self->complete_callback(self->complete_user_data,
                            ut_object_ref(self->error));
  }
}

static void process_auth(UtDBusAuthServer *self, const char *args) {
  char *mechanism_end = strchr(args, ' ');
  ut_cstring_ref mechanism = NULL;
  ut_cstring_ref auth_args = NULL;
  if (mechanism_end == NULL) {
    mechanism = ut_cstring_new(args);
    auth_args = ut_cstring_new("");
  } else {
    mechanism = ut_cstring_new_sized(args, mechanism_end - args);
    auth_args = ut_cstring_new(mechanism_end + 1);
  }

  if (ut_cstring_equal(mechanism, "")) {
    send_auth_message(self, "REJECTED EXTERNAL");
  } else if (ut_cstring_equal(mechanism, "EXTERNAL")) {
    // FIXME: Actually to the authentication...
    self->authenticated = true;
    send_auth_message(self, "OK");
  } else {
    send_auth_message(self, "REJECTED Mechanism not supported");
  }
}

static void process_begin(UtDBusAuthServer *self, const char *args) {
  if (!self->authenticated) {
    send_auth_message(self, "ERROR");
    return;
  }

  done(self);
}

static void process_line(UtDBusAuthServer *self, const char *line) {
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

  if (ut_cstring_equal(command, "AUTH")) {
    process_auth(self, args);
  } else if (ut_cstring_equal(command, "BEGIN")) {
    process_begin(self, args);
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

static size_t read_cb(void *user_data, UtObject *data, bool complete) {
  UtDBusAuthServer *self = user_data;

  size_t offset = 0;

  // Read empty byte, which was used for sending credentials (no longer
  // required).
  if (self->state == AUTH_STATE_CREDENTIALS) {
    if (ut_list_get_length(data) < 1) {
      return 0;
    }
    // FIXME: Validate is a zero byte
    offset++;
    self->state = AUTH_STATE_READY;
  }

  while (self->state != AUTH_STATE_DONE) {
    ut_cstring_ref line = read_line(data, &offset);
    if (line == NULL) {
      break;
    }

    process_line(self, line);
  }

  return offset;
}

static void ut_dbus_auth_server_init(UtObject *object) {
  UtDBusAuthServer *self = (UtDBusAuthServer *)object;
  self->read_cancel = ut_cancel_new();
  self->state = AUTH_STATE_IDLE;
}

static void ut_dbus_auth_server_cleanup(UtObject *object) {
  UtDBusAuthServer *self = (UtDBusAuthServer *)object;
  ut_object_unref(self->input_stream);
  ut_object_unref(self->output_stream);
  ut_object_unref(self->read_cancel);
  ut_object_unref(self->error);
  ut_object_unref(self->complete_cancel);
}

static UtObjectInterface object_interface = {.type_name = "UtDBusAuthServer",
                                             .init = ut_dbus_auth_server_init,
                                             .cleanup =
                                                 ut_dbus_auth_server_cleanup,
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_dbus_auth_server_new(UtObject *input_stream,
                                  UtObject *output_stream) {
  UtObject *object = ut_object_new(sizeof(UtDBusAuthServer), &object_interface);
  UtDBusAuthServer *self = (UtDBusAuthServer *)object;
  self->input_stream = ut_object_ref(input_stream);
  self->output_stream = ut_object_ref(output_stream);
  return object;
}

void ut_dbus_auth_server_run(UtObject *object, UtAuthCompleteCallback callback,
                             void *user_data, UtObject *cancel) {
  assert(ut_object_is_dbus_auth_server(object));
  UtDBusAuthServer *self = (UtDBusAuthServer *)object;

  assert(self->state == AUTH_STATE_IDLE);
  self->state = AUTH_STATE_CREDENTIALS;

  assert(callback != NULL);
  self->complete_callback = callback;
  self->complete_user_data = user_data;
  self->complete_cancel = ut_object_ref(cancel);

  ut_input_stream_read(self->input_stream, read_cb, self, self->read_cancel);
}

bool ut_object_is_dbus_auth_server(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
