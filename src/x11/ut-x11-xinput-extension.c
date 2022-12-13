#include <assert.h>

#include "ut-x11-buffer.h"
#include "ut-x11-client-private.h"
#include "ut-x11-extension.h"
#include "ut-x11-xinput-extension.h"
#include "ut.h"

typedef struct {
  UtObject object;
  void *callback;
  void *user_data;
} CallbackData;

static UtObjectInterface callback_data_object_interface = {
    .type_name = "XinputCallbackData", .interfaces = {{NULL, NULL}}};

static UtObject *callback_data_new(void *callback, void *user_data) {
  UtObject *object =
      ut_object_new(sizeof(CallbackData), &callback_data_object_interface);
  CallbackData *self = (CallbackData *)object;
  self->callback = callback;
  self->user_data = user_data;
  return object;
}

typedef struct {
  UtObject object;
  UtObject *client;
  uint8_t major_opcode;
  uint8_t first_event;
  uint8_t first_error;
  const UtX11EventCallbacks *event_callbacks;
  void *user_data;
  UtObject *cancel;
} UtX11XinputExtension;

static void query_version_reply_cb(UtObject *object, uint8_t data0,
                                   UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  ut_x11_buffer_get_padding(data, &offset, 1);
  uint16_t major_version = ut_x11_buffer_get_card16(data, &offset);
  uint16_t minor_version = ut_x11_buffer_get_card16(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 20);

  if (callback_data->callback != NULL) {
    UtX11ClientXinputQueryVersionCallback callback =
        (UtX11ClientXinputQueryVersionCallback)callback_data->callback;
    callback(callback_data->user_data, major_version, minor_version, NULL);
  }
}

static void query_version_error_cb(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback != NULL) {
    UtX11ClientXinputQueryVersionCallback callback =
        (UtX11ClientXinputQueryVersionCallback)callback_data->callback;
    callback(callback_data->user_data, 0, 0, error);
  }
}

static void ut_x11_xinput_extension_cleanup(UtObject *object) {
  UtX11XinputExtension *self = (UtX11XinputExtension *)object;
  ut_object_unref(self->cancel);
}

static uint8_t ut_x11_xinput_extension_get_major_opcode(UtObject *object) {
  UtX11XinputExtension *self = (UtX11XinputExtension *)object;
  return self->major_opcode;
}

static bool ut_x11_xinput_extension_decode_event(UtObject *object,
                                                 UtObject *data) {
  UtX11XinputExtension *self = (UtX11XinputExtension *)object;

  size_t offset = 0;
  uint8_t code = ut_x11_buffer_get_card8(data, &offset) & 0x7f;
  if (code < self->first_event) {
    return false;
  }
  code -= self->first_event;

  switch (code) {
  default:
    return false;
  }
}

static UtObject *ut_x11_xinput_extension_decode_error(UtObject *object,
                                                      UtObject *data) {
  // FIXME
  return NULL;
}

static void ut_x11_xinput_extension_close(UtObject *object) {
  UtX11XinputExtension *self = (UtX11XinputExtension *)object;
  self->client = NULL;
}

static UtX11ExtensionInterface x11_extension_interface = {
    .get_major_opcode = ut_x11_xinput_extension_get_major_opcode,
    .decode_event = ut_x11_xinput_extension_decode_event,
    .decode_error = ut_x11_xinput_extension_decode_error,
    .close = ut_x11_xinput_extension_close};

static UtObjectInterface object_interface = {
    .type_name = "UtX11XinputExtension",
    .cleanup = ut_x11_xinput_extension_cleanup,
    .interfaces = {{&ut_x11_extension_id, &x11_extension_interface},
                   {NULL, NULL}}};

UtObject *
ut_x11_xinput_extension_new(UtObject *client, uint8_t major_opcode,
                            uint8_t first_event, uint8_t first_error,
                            const UtX11EventCallbacks *event_callbacks,
                            void *user_data, UtObject *cancel) {
  UtObject *object =
      ut_object_new(sizeof(UtX11XinputExtension), &object_interface);
  UtX11XinputExtension *self = (UtX11XinputExtension *)object;
  self->client = client;
  self->major_opcode = major_opcode;
  self->first_event = first_event;
  self->first_error = first_error;
  self->event_callbacks = event_callbacks;
  self->user_data = user_data;
  self->cancel = ut_object_ref(cancel);
  return object;
}

void ut_x11_xinput_extension_query_version(
    UtObject *object, UtX11ClientXinputQueryVersionCallback callback,
    void *user_data, UtObject *cancel) {
  assert(ut_object_is_x11_xinput_extension(object));
  UtX11XinputExtension *self = (UtX11XinputExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card16(request, 2);
  ut_x11_buffer_append_card16(request, 4);

  ut_x11_client_send_request_with_reply(
      self->client, self->major_opcode, 47, request, query_version_reply_cb,
      query_version_error_cb, callback_data_new(callback, user_data), cancel);
}

bool ut_object_is_x11_xinput_extension(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
