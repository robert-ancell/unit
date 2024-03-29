#include <assert.h>

#include "ut-x11-buffer.h"
#include "ut-x11-client-private.h"
#include "ut-x11-extension.h"
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
    .type_name = "BigRequestsCallbackData", .cleanup = callback_data_cleanup};

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
} UtX11BigRequestsExtension;

static void enable_reply_cb(UtObject *object, uint8_t data0, UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  ut_x11_buffer_get_padding(data, &offset, 1);
  uint32_t maximum_request_length = ut_x11_buffer_get_card32(data, &offset);

  if (callback_data->callback_object != NULL &&
      callback_data->callback != NULL) {
    UtX11ClientBigRequestsEnableCallback callback =
        (UtX11ClientBigRequestsEnableCallback)callback_data->callback;
    callback(callback_data->callback_object, maximum_request_length, NULL);
  }
}

static void enable_error_cb(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback_object != NULL &&
      callback_data->callback != NULL) {
    UtX11ClientBigRequestsEnableCallback callback =
        (UtX11ClientBigRequestsEnableCallback)callback_data->callback;
    callback(callback_data->callback_object, 0, error);
  }
}

static uint8_t ut_x11_big_request_extension_get_major_opcode(UtObject *object) {
  UtX11BigRequestsExtension *self = (UtX11BigRequestsExtension *)object;
  return self->major_opcode;
}

static void ut_x11_big_requests_extension_close(UtObject *object) {
  UtX11BigRequestsExtension *self = (UtX11BigRequestsExtension *)object;
  self->client = NULL;
}

static UtX11ExtensionInterface x11_extension_interface = {
    .get_major_opcode = ut_x11_big_request_extension_get_major_opcode,
    .close = ut_x11_big_requests_extension_close};

static UtObjectInterface object_interface = {
    .type_name = "UtX11BigRequestsExtension",
    .interfaces = {{&ut_x11_extension_id, &x11_extension_interface},
                   {NULL, NULL}}};

UtObject *ut_x11_big_requests_extension_new(UtObject *client,
                                            uint8_t major_opcode) {
  UtObject *object =
      ut_object_new(sizeof(UtX11BigRequestsExtension), &object_interface);
  UtX11BigRequestsExtension *self = (UtX11BigRequestsExtension *)object;
  self->client = client;
  self->major_opcode = major_opcode;
  return object;
}

void ut_x11_big_requests_extension_enable(
    UtObject *object, UtObject *callback_object,
    UtX11ClientBigRequestsEnableCallback callback) {
  assert(ut_object_is_x11_big_requests_extension(object));
  UtX11BigRequestsExtension *self = (UtX11BigRequestsExtension *)object;

  ut_x11_client_send_request_with_reply(
      self->client, self->major_opcode, 0, NULL,
      callback_data_new(callback_object, callback), enable_reply_cb,
      enable_error_cb);
}

bool ut_object_is_x11_big_requests_extension(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
