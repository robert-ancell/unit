#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef enum {
  UT_DBUS_MESSAGE_TYPE_METHOD_CALL = 1,
  UT_DBUS_MESSAGE_TYPE_METHOD_RETURN = 2,
  UT_DBUS_MESSAGE_TYPE_ERROR = 3,
  UT_DBUS_MESSAGE_TYPE_SIGNAL = 4
} UtDBusMessageType;

typedef enum {
  UT_DBUS_MESSAGE_FLAG_NO_REPLY_EXPECTED = 0x01,
  UT_DBUS_MESSAGE_FLAG_NO_AUTO_START = 0x02,
  UT_DBUS_MESSAGE_FLAG_ALLOW_INTERACTIVE_AUTHORIZATION = 0x04
} UtDBusMessageFlag;

/// Creates a new DBus message of [type].
///
/// !return-ref
/// !return-type UtDbusMessage
UtObject *ut_dbus_message_new(UtDBusMessageType type);

/// Creates a new DBus method call message sending to [destination], object on
/// [path], callinging [interface].[name] with optional [args].
///
/// !arg-type args UtObjectList NULL
/// !return-ref
/// !return-type UtDbusMessage
UtObject *ut_dbus_message_new_method_call(const char *destination,
                                          const char *path,
                                          const char *interface,
                                          const char *name, UtObject *args);

/// Creates a new DBus method call return message responsing to [reply_serial]
/// and containing returned optional [args].
///
/// !arg-type args UtObjectList NULL
/// !return-ref
/// !return-type UtDbusMessage
UtObject *ut_dbus_message_new_method_return(uint32_t reply_serial,
                                            UtObject *args);

/// Creates a new DBus error message with [error_name] and optional [values].
///
/// !arg-type values UtObjectList NULL
/// !return-ref
/// !return-type UtDbusMessage
UtObject *ut_dbus_message_new_error(const char *error_name, UtObject *values);

/// Create a new DBus signal message sending from object on [path],
/// [interface].[name] and optional [args].
///
/// !arg-type args UtObjectList NULL
/// !return-ref
/// !return-type UtDbusMessage
UtObject *ut_dbus_message_new_signal(const char *path, const char *interface,
                                     const char *name, UtObject *args);

/// Returns the type of DBus message this is.
UtDBusMessageType ut_dbus_message_get_type(UtObject *object);

/// Sets the [flags] for this DBus message.
void ut_dbus_message_set_flags(UtObject *object, UtDBusMessageFlag flags);

/// Returns the flags for this DBus message.
UtDBusMessageFlag ut_dbus_message_get_flags(UtObject *object);

/// Sets the [serial] number for this message.
void ut_dbus_message_set_serial(UtObject *object, uint32_t serial);

/// Returns the serial number for this message.
uint32_t ut_dbus_message_get_serial(UtObject *object);

/// Sets the [destination] for this message.
void ut_dbus_message_set_destination(UtObject *object, const char *destination);

/// Returns the destination for this message.
const char *ut_dbus_message_get_destination(UtObject *object);

/// Sets the [sender] for this message.
void ut_dbus_message_set_sender(UtObject *object, const char *sender);

/// Returns the sender for this message.
const char *ut_dbus_message_get_sender(UtObject *object);

/// Sets the [path] for this message.
void ut_dbus_message_set_path(UtObject *object, const char *path);

/// Returns the path for this message.
const char *ut_dbus_message_get_path(UtObject *object);

/// Sets the [interface] for this message.
void ut_dbus_message_set_interface(UtObject *object, const char *interface);

/// Returns the interface for this message.
const char *ut_dbus_message_get_interface(UtObject *object);

/// Sets the [member] for this message.
void ut_dbus_message_set_member(UtObject *object, const char *member);

/// Returns the member for this message.
const char *ut_dbus_message_get_member(UtObject *object);

/// Sets the [error_name] for this message.
void ut_dbus_message_set_error_name(UtObject *object, const char *error_name);

/// Returns the error name for this message.
const char *ut_dbus_message_get_error_name(UtObject *object);

/// Sets the [serial] number this message is replying to.
void ut_dbus_message_set_reply_serial(UtObject *object, uint32_t serial);

/// Returns true if this message is replying to a serial number.
bool ut_dbus_message_has_reply_serial(UtObject *object);

/// Returns the serial number this message is replying to.
uint32_t ut_dbus_message_get_reply_serial(UtObject *object);

/// Sets the [args] that are passed in this message.
///
/// !arg-type args UtObjectList
void ut_dbus_message_set_args(UtObject *object, UtObject *args);

/// Returns the args that are passed in this message.
///
/// !return-type args UtObjectList NULL
UtObject *ut_dbus_message_get_args(UtObject *object);

/// Returns [true] if [object] is a [UtDbusMessage].
bool ut_object_is_dbus_message(UtObject *object);
