#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  uint8_t type;
  uint8_t flags;
  uint32_t serial;
  char *path;
  char *interface;
  char *member;
  char *error_name;
  bool has_reply_serial;
  uint32_t reply_serial;
  char *destination;
  char *sender;
  UtObject *args;
} UtDBusMessage;

static char *ut_dbus_message_to_string(UtObject *object) {
  UtDBusMessage *self = (UtDBusMessage *)object;
  UtObjectRef parameters = ut_string_list_new();
  ut_string_list_append_printf(parameters, "type=%d", self->type);
  if (self->flags != 0) {
    ut_string_list_append_printf(parameters, "flags=%02x", self->flags);
  }
  ut_string_list_append_printf(parameters, "serial=%d", self->serial);
  if (self->path != NULL) {
    ut_string_list_append_printf(parameters, "path=%s", self->path);
  }
  if (self->interface != NULL) {
    ut_string_list_append_printf(parameters, "interface=%s", self->interface);
  }
  if (self->member != NULL) {
    ut_string_list_append_printf(parameters, "member=%s", self->member);
  }
  if (self->error_name != NULL) {
    ut_string_list_append_printf(parameters, "error_name=%s", self->error_name);
  }
  if (self->has_reply_serial) {
    ut_string_list_append_printf(parameters, "reply_serial=%d",
                                 self->reply_serial);
  }
  if (self->destination != NULL) {
    ut_string_list_append_printf(parameters, "destination=%s",
                                 self->destination);
  }
  if (self->sender != NULL) {
    ut_string_list_append_printf(parameters, "sender=%s", self->sender);
  }
  ut_cstring_ref parameter_list = ut_string_list_join(parameters, ", ");
  return ut_cstring_new_printf("<UtDBusMessage>(%s)", parameter_list);
}

static void ut_dbus_message_cleanup(UtObject *object) {
  UtDBusMessage *self = (UtDBusMessage *)object;
  free(self->path);
  free(self->interface);
  free(self->member);
  free(self->error_name);
  free(self->destination);
  free(self->sender);
  ut_object_unref(self->args);
}

static UtObjectInterface object_interface = {.type_name = "UtDBusMessage",
                                             .to_string =
                                                 ut_dbus_message_to_string,
                                             .cleanup = ut_dbus_message_cleanup,
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_dbus_message_new(uint8_t type) {
  UtObject *object = ut_object_new(sizeof(UtDBusMessage), &object_interface);
  UtDBusMessage *self = (UtDBusMessage *)object;
  self->type = type;
  return object;
}

UtObject *ut_dbus_message_new_method_call(const char *destination,
                                          const char *path,
                                          const char *interface,
                                          const char *name, UtObject *args) {
  UtObject *object = ut_object_new(sizeof(UtDBusMessage), &object_interface);
  UtDBusMessage *self = (UtDBusMessage *)object;
  self->type = UT_DBUS_MESSAGE_TYPE_METHOD_CALL;
  self->destination = ut_cstring_new(destination);
  self->path = ut_cstring_new(path);
  self->interface = ut_cstring_new(interface);
  self->member = ut_cstring_new(name);
  self->args = ut_object_ref(args);
  return object;
}

UtObject *ut_dbus_message_new_method_return(uint32_t reply_serial,
                                            UtObject *out_args) {
  UtObject *object = ut_object_new(sizeof(UtDBusMessage), &object_interface);
  UtDBusMessage *self = (UtDBusMessage *)object;
  self->type = UT_DBUS_MESSAGE_TYPE_METHOD_RETURN;
  self->has_reply_serial = true;
  self->reply_serial = reply_serial;
  self->args = ut_object_ref(out_args);
  return object;
}

UtObject *ut_dbus_message_new_error(const char *error_name, UtObject *args) {
  UtObject *object = ut_object_new(sizeof(UtDBusMessage), &object_interface);
  UtDBusMessage *self = (UtDBusMessage *)object;
  self->type = UT_DBUS_MESSAGE_TYPE_ERROR;
  self->error_name = ut_cstring_new(error_name);
  self->args = ut_object_ref(args);
  return object;
}

UtObject *ut_dbus_message_new_signal(const char *path, const char *interface,
                                     const char *name, UtObject *args) {
  UtObject *object = ut_object_new(sizeof(UtDBusMessage), &object_interface);
  UtDBusMessage *self = (UtDBusMessage *)object;
  self->type = UT_DBUS_MESSAGE_TYPE_SIGNAL;
  self->path = ut_cstring_new(path);
  self->interface = ut_cstring_new(interface);
  self->member = ut_cstring_new(name);
  self->args = ut_object_ref(args);
  return object;
}

UtDBusMessageType ut_dbus_message_get_type(UtObject *object) {
  assert(ut_object_is_dbus_message(object));
  UtDBusMessage *self = (UtDBusMessage *)object;
  return self->type;
}

void ut_dbus_message_set_flags(UtObject *object, uint8_t flags) {
  assert(ut_object_is_dbus_message(object));
  UtDBusMessage *self = (UtDBusMessage *)object;
  self->flags = flags;
}

uint8_t ut_dbus_message_get_flags(UtObject *object) {
  assert(ut_object_is_dbus_message(object));
  UtDBusMessage *self = (UtDBusMessage *)object;
  return self->flags;
}

void ut_dbus_message_set_serial(UtObject *object, uint32_t serial) {
  assert(ut_object_is_dbus_message(object));
  UtDBusMessage *self = (UtDBusMessage *)object;
  self->serial = serial;
}

uint32_t ut_dbus_message_get_serial(UtObject *object) {
  assert(ut_object_is_dbus_message(object));
  UtDBusMessage *self = (UtDBusMessage *)object;
  return self->serial;
}

void ut_dbus_message_set_destination(UtObject *object,
                                     const char *destination) {
  assert(ut_object_is_dbus_message(object));
  UtDBusMessage *self = (UtDBusMessage *)object;
  ut_cstring_set(&self->destination, destination);
}

const char *ut_dbus_message_get_destination(UtObject *object) {
  assert(ut_object_is_dbus_message(object));
  UtDBusMessage *self = (UtDBusMessage *)object;
  return self->destination;
}

void ut_dbus_message_set_sender(UtObject *object, const char *sender) {
  assert(ut_object_is_dbus_message(object));
  UtDBusMessage *self = (UtDBusMessage *)object;
  ut_cstring_set(&self->sender, sender);
}

const char *ut_dbus_message_get_sender(UtObject *object) {
  assert(ut_object_is_dbus_message(object));
  UtDBusMessage *self = (UtDBusMessage *)object;
  return self->sender;
}

void ut_dbus_message_set_path(UtObject *object, const char *path) {
  assert(ut_object_is_dbus_message(object));
  UtDBusMessage *self = (UtDBusMessage *)object;
  ut_cstring_set(&self->path, path);
}

const char *ut_dbus_message_get_path(UtObject *object) {
  assert(ut_object_is_dbus_message(object));
  UtDBusMessage *self = (UtDBusMessage *)object;
  return self->path;
}

void ut_dbus_message_set_interface(UtObject *object, const char *interface) {
  assert(ut_object_is_dbus_message(object));
  UtDBusMessage *self = (UtDBusMessage *)object;
  ut_cstring_set(&self->interface, interface);
}

const char *ut_dbus_message_get_interface(UtObject *object) {
  assert(ut_object_is_dbus_message(object));
  UtDBusMessage *self = (UtDBusMessage *)object;
  return self->interface;
}

void ut_dbus_message_set_member(UtObject *object, const char *member) {
  assert(ut_object_is_dbus_message(object));
  UtDBusMessage *self = (UtDBusMessage *)object;
  ut_cstring_set(&self->member, member);
}

const char *ut_dbus_message_get_member(UtObject *object) {
  assert(ut_object_is_dbus_message(object));
  UtDBusMessage *self = (UtDBusMessage *)object;
  return self->member;
}

void ut_dbus_message_set_error_name(UtObject *object, const char *error_name) {
  assert(ut_object_is_dbus_message(object));
  UtDBusMessage *self = (UtDBusMessage *)object;
  ut_cstring_set(&self->error_name, error_name);
}

const char *ut_dbus_message_get_error_name(UtObject *object) {
  assert(ut_object_is_dbus_message(object));
  UtDBusMessage *self = (UtDBusMessage *)object;
  return self->error_name;
}

void ut_dbus_message_set_reply_serial(UtObject *object, uint32_t reply_serial) {
  assert(ut_object_is_dbus_message(object));
  UtDBusMessage *self = (UtDBusMessage *)object;
  self->has_reply_serial = true;
  self->reply_serial = reply_serial;
}

bool ut_dbus_message_has_reply_serial(UtObject *object) {
  assert(ut_object_is_dbus_message(object));
  UtDBusMessage *self = (UtDBusMessage *)object;
  return self->has_reply_serial;
}

uint32_t ut_dbus_message_get_reply_serial(UtObject *object) {
  assert(ut_object_is_dbus_message(object));
  UtDBusMessage *self = (UtDBusMessage *)object;
  return self->reply_serial;
}

void ut_dbus_message_set_args(UtObject *object, UtObject *args) {
  assert(ut_object_is_dbus_message(object));
  UtDBusMessage *self = (UtDBusMessage *)object;
  ut_object_set(&self->args, args);
}

UtObject *ut_dbus_message_get_args(UtObject *object) {
  assert(ut_object_is_dbus_message(object));
  UtDBusMessage *self = (UtDBusMessage *)object;
  return self->args;
}

bool ut_object_is_dbus_message(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
