#include <assert.h>

#include "ut-x11-buffer.h"
#include "ut-x11-client-private.h"
#include "ut-x11-extension.h"
#include "ut-x11-generic-event-extension.h"
#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *callback_object;
  void *callback;
} CallbackData;

static void callback_data_cleanup(UtObject *object) {
  CallbackData *self = (CallbackData *)object;
  ut_object_weak_unref(&self->callback_object);
}

static UtObjectInterface callback_data_object_interface = {
    .type_name = "GenericEventCallbackData", .cleanup = callback_data_cleanup};

static UtObject *callback_data_new(UtObject *callback_object, void *callback) {
  UtObject *object =
      ut_object_new(sizeof(CallbackData), &callback_data_object_interface);
  CallbackData *self = (CallbackData *)object;
  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callback = callback;
  return object;
}

typedef struct {
  UtObject object;
  UtObject *client;
  uint8_t major_opcode;
} UtX11GenericEventExtension;

static void query_version_reply_cb(UtObject *object, uint8_t data0,
                                   UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  ut_x11_buffer_get_padding(data, &offset, 1);
  uint16_t major_version = ut_x11_buffer_get_card16(data, &offset);
  uint16_t minor_version = ut_x11_buffer_get_card16(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 20);

  if (callback_data->callback_object != NULL &&
      callback_data->callback != NULL) {
    UtX11ClientGenericEventQueryVersionCallback callback =
        (UtX11ClientGenericEventQueryVersionCallback)callback_data->callback;
    callback(callback_data->callback_object, major_version, minor_version,
             NULL);
  }
}

static void query_version_error_cb(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback_object != NULL &&
      callback_data->callback != NULL) {
    UtX11ClientGenericEventQueryVersionCallback callback =
        (UtX11ClientGenericEventQueryVersionCallback)callback_data->callback;
    callback(callback_data->callback_object, 0, 0, error);
  }
}

static uint8_t
ut_x11_generic_event_extension_get_major_opcode(UtObject *object) {
  UtX11GenericEventExtension *self = (UtX11GenericEventExtension *)object;
  return self->major_opcode;
}

static void ut_x11_generic_event_extension_close(UtObject *object) {
  UtX11GenericEventExtension *self = (UtX11GenericEventExtension *)object;
  self->client = NULL;
}

static UtX11ExtensionInterface x11_extension_interface = {
    .get_major_opcode = ut_x11_generic_event_extension_get_major_opcode,
    .close = ut_x11_generic_event_extension_close};

static UtObjectInterface object_interface = {
    .type_name = "UtX11GenericEventExtension",
    .interfaces = {{&ut_x11_extension_id, &x11_extension_interface},
                   {NULL, NULL}}};

UtObject *ut_x11_generic_event_extension_new(UtObject *client,
                                             uint8_t major_opcode) {
  UtObject *object =
      ut_object_new(sizeof(UtX11GenericEventExtension), &object_interface);
  UtX11GenericEventExtension *self = (UtX11GenericEventExtension *)object;
  self->client = client;
  self->major_opcode = major_opcode;
  return object;
}

void ut_x11_generic_event_extension_query_version(
    UtObject *object, UtObject *callback_object,
    UtX11ClientGenericEventQueryVersionCallback callback) {
  assert(ut_object_is_x11_generic_event_extension(object));
  UtX11GenericEventExtension *self = (UtX11GenericEventExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card16(request, 1);
  ut_x11_buffer_append_card16(request, 0);

  ut_x11_client_send_request_with_reply(
      self->client, self->major_opcode, 0, request,
      callback_data_new(callback_object, callback), query_version_reply_cb,
      query_version_error_cb);
}

bool ut_object_is_x11_generic_event_extension(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
