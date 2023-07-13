#include <assert.h>

#include "ut-x11-buffer.h"
#include "ut-x11-client-private.h"
#include "ut-x11-extension.h"
#include "ut-x11-randr-extension.h"
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
    .type_name = "RandrCallbackData", .cleanup = callback_data_cleanup};

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
  uint8_t first_event;
  uint8_t first_error;
  UtObject *callback_object;
  const UtX11EventCallbacks *event_callbacks;
} UtX11RandrExtension;

static void query_version_reply_cb(UtObject *object, uint8_t data0,
                                   UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  uint32_t major_version = ut_x11_buffer_get_card32(data, &offset);
  uint32_t minor_version = ut_x11_buffer_get_card32(data, &offset);

  if (callback_data->callback_object != NULL &&
      callback_data->callback != NULL) {
    UtX11ClientRandrQueryVersionCallback callback =
        (UtX11ClientRandrQueryVersionCallback)callback_data->callback;
    callback(callback_data->callback_object, major_version, minor_version,
             NULL);
  }
}

static void query_version_error_cb(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback_object != NULL &&
      callback_data->callback != NULL) {
    UtX11ClientRandrQueryVersionCallback callback =
        (UtX11ClientRandrQueryVersionCallback)callback_data->callback;
    callback(callback_data->callback_object, 0, 0, error);
  }
}

static uint8_t ut_x11_randr_extension_get_major_opcode(UtObject *object) {
  UtX11RandrExtension *self = (UtX11RandrExtension *)object;
  return self->major_opcode;
}

static uint8_t ut_x11_randr_extension_get_first_event(UtObject *object) {
  UtX11RandrExtension *self = (UtX11RandrExtension *)object;
  return self->first_event;
}

static uint8_t ut_x11_randr_extension_get_first_error(UtObject *object) {
  UtX11RandrExtension *self = (UtX11RandrExtension *)object;
  return self->first_error;
}

static bool ut_x11_randr_extension_decode_event(UtObject *object, uint8_t code,
                                                bool from_send_event,
                                                uint16_t sequence_number,
                                                uint8_t data0, UtObject *data) {
  return false;
}

static UtX11ErrorCode ut_x11_randr_extension_decode_error(UtObject *object,
                                                          uint8_t code) {
  switch (code) {
  case 0:
    return UT_X11_ERROR_BAD_OUTPUT;
  case 1:
    return UT_X11_ERROR_BAD_CRTC;
  case 2:
    return UT_X11_ERROR_BAD_MODE;
  case 3:
    return UT_X11_ERROR_BAD_PROVIDER;
  default:
    return UT_X11_ERROR_UNKNOWN;
  }
}

static void ut_x11_randr_extension_close(UtObject *object) {
  UtX11RandrExtension *self = (UtX11RandrExtension *)object;
  self->client = NULL;
}

static void ut_x11_randr_extension_cleanup(UtObject *object) {
  UtX11RandrExtension *self = (UtX11RandrExtension *)object;
  ut_object_weak_unref(&self->callback_object);
}

static UtX11ExtensionInterface x11_extension_interface = {
    .get_major_opcode = ut_x11_randr_extension_get_major_opcode,
    .get_first_event = ut_x11_randr_extension_get_first_event,
    .get_first_error = ut_x11_randr_extension_get_first_error,
    .decode_event = ut_x11_randr_extension_decode_event,
    .decode_error = ut_x11_randr_extension_decode_error,
    .close = ut_x11_randr_extension_close};

static UtObjectInterface object_interface = {
    .type_name = "UtX11RandrExtension",
    .cleanup = ut_x11_randr_extension_cleanup,
    .interfaces = {{&ut_x11_extension_id, &x11_extension_interface},
                   {NULL, NULL}}};

UtObject *
ut_x11_randr_extension_new(UtObject *client, uint8_t major_opcode,
                           uint8_t first_event, uint8_t first_error,
                           UtObject *callback_object,
                           const UtX11EventCallbacks *event_callbacks) {
  UtObject *object =
      ut_object_new(sizeof(UtX11RandrExtension), &object_interface);
  UtX11RandrExtension *self = (UtX11RandrExtension *)object;
  self->client = client;
  self->major_opcode = major_opcode;
  self->first_event = first_event;
  self->first_error = first_error;
  ut_object_weak_ref(callback_object, &self->callback_object);
  self->event_callbacks = event_callbacks;
  return object;
}

void ut_x11_randr_extension_query_version(
    UtObject *object, UtObject *callback_object,
    UtX11ClientRandrQueryVersionCallback callback) {
  assert(ut_object_is_x11_randr_extension(object));
  UtX11RandrExtension *self = (UtX11RandrExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, 1);
  ut_x11_buffer_append_card32(request, 5);

  ut_x11_client_send_request_with_reply(
      self->client, self->major_opcode, 0, request,
      callback_data_new(callback_object, callback), query_version_reply_cb,
      query_version_error_cb);
}

bool ut_object_is_x11_randr_extension(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
