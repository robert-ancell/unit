#include <assert.h>

#include "ut-x11-buffer.h"
#include "ut-x11-client-private.h"
#include "ut-x11-extension.h"
#include "ut-x11-randr-extension.h"
#include "ut.h"

typedef struct {
  UtObject object;
  void *callback;
  void *user_data;
} CallbackData;

static UtObjectInterface callback_data_object_interface = {
    .type_name = "RandrCallbackData", .interfaces = {{NULL, NULL}}};

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
} UtX11RandrExtension;

static void query_version_reply_cb(UtObject *object, uint8_t data0,
                                   UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  uint32_t major_version = ut_x11_buffer_get_card32(data, &offset);
  uint32_t minor_version = ut_x11_buffer_get_card32(data, &offset);

  if (callback_data->callback != NULL) {
    UtX11ClientRandrQueryVersionCallback callback =
        (UtX11ClientRandrQueryVersionCallback)callback_data->callback;
    callback(callback_data->user_data, major_version, minor_version, NULL);
  }
}

static void query_version_error_cb(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback != NULL) {
    UtX11ClientRandrQueryVersionCallback callback =
        (UtX11ClientRandrQueryVersionCallback)callback_data->callback;
    callback(callback_data->user_data, 0, 0, error);
  }
}

static void ut_x11_randr_extension_cleanup(UtObject *object) {
  UtX11RandrExtension *self = (UtX11RandrExtension *)object;
  ut_object_unref(self->cancel);
}

static bool ut_x11_randr_extension_decode_event(UtObject *object,
                                                UtObject *data) {
  return false;
}

static UtObject *ut_x11_randr_extension_decode_error(UtObject *object,
                                                     UtObject *data) {
  return NULL;
}

static void ut_x11_randr_extension_close(UtObject *object) {
  UtX11RandrExtension *self = (UtX11RandrExtension *)object;
  self->client = NULL;
}

static UtX11ExtensionInterface x11_extension_interface = {
    .decode_event = ut_x11_randr_extension_decode_event,
    .decode_error = ut_x11_randr_extension_decode_error,
    .close = ut_x11_randr_extension_close};

static UtObjectInterface object_interface = {
    .type_name = "UtX11RandrExtension",
    .cleanup = ut_x11_randr_extension_cleanup,
    .interfaces = {{&ut_x11_extension_id, &x11_extension_interface},
                   {NULL, NULL}}};

UtObject *ut_x11_randr_extension_new(UtObject *client, uint8_t major_opcode,
                                     uint8_t first_event, uint8_t first_error,
                                     const UtX11EventCallbacks *event_callbacks,
                                     void *user_data, UtObject *cancel) {
  UtObject *object =
      ut_object_new(sizeof(UtX11RandrExtension), &object_interface);
  UtX11RandrExtension *self = (UtX11RandrExtension *)object;
  self->client = client;
  self->major_opcode = major_opcode;
  self->first_event = first_event;
  self->first_error = first_error;
  self->event_callbacks = event_callbacks;
  self->user_data = user_data;
  self->cancel = ut_object_ref(cancel);
  return object;
}

void ut_x11_randr_extension_query_version(
    UtObject *object, UtX11ClientRandrQueryVersionCallback callback,
    void *user_data, UtObject *cancel) {
  assert(ut_object_is_x11_randr_extension(object));
  UtX11RandrExtension *self = (UtX11RandrExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, 1);
  ut_x11_buffer_append_card32(request, 5);

  ut_x11_client_send_request_with_reply(
      self->client, self->major_opcode, 0, request, query_version_reply_cb,
      query_version_error_cb, callback_data_new(callback, user_data), cancel);
}

void ut_x11_randr_extension_set_screen_config(
    UtObject *object, uint32_t window, uint32_t timestamp,
    uint32_t config_timestamp, int16_t size_index, uint16_t rotation,
    UtX11ClientRandrSetScreenConfigCallback callback, void *user_data,
    UtObject *cancel) {
  assert(ut_object_is_x11_randr_extension(object));
  UtX11RandrExtension *self = (UtX11RandrExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  // FIXME

  // ut_x11_client_send_request_with_reply(
  //     self->client, self->major_opcode, 2, request, query_version_reply_cb,
  //     query_version_error_cb, callback_data_new(callback, user_data),
  //     cancel);
}

void ut_x11_randr_extension_select_input(UtObject *object, uint32_t window,
                                         uint16_t enable) {
  assert(ut_object_is_x11_randr_extension(object));
  UtX11RandrExtension *self = (UtX11RandrExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);
  ut_x11_buffer_append_card32(request, enable);
  ut_x11_buffer_append_padding(request, 2);

  ut_x11_client_send_request(self->client, self->major_opcode, 4, request);
}

void ut_x11_randr_extension_get_screen_info(
    UtObject *object, uint32_t window,
    UtX11ClientRandrGetScreenInfoCallback callback, void *user_data,
    UtObject *cancel) {
  assert(ut_object_is_x11_randr_extension(object));
  UtX11RandrExtension *self = (UtX11RandrExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);

  // ut_x11_client_send_request_with_reply(
  //     self->client, self->major_opcode, 5, request, query_version_reply_cb,
  //     query_version_error_cb, callback_data_new(callback, user_data),
  //     cancel);
}

void ut_x11_randr_extension_get_screen_size_range(
    UtObject *object, uint32_t window,
    UtX11ClientRandrGetScreenSizeRangeCallback callback, void *user_data,
    UtObject *cancel) {}

void ut_x11_randr_extension_set_screen_size(UtObject *object, uint32_t window,
                                            uint16_t width, uint16_t height,
                                            uint32_t width_in_millimeters,
                                            uint32_t height_in_millimeters) {}

void ut_x11_randr_extension_get_screen_resources(
    UtObject *object, uint32_t window,
    UtX11ClientRandrGetScreenResourcesCallback callback, void *user_data,
    UtObject *cancel) {}

void ut_x11_randr_extension_get_output_info(
    UtObject *object, uint32_t output, uint32_t config_timestamp,
    UtX11ClientRandrGetOutputInfoCallback callback, void *user_data,
    UtObject *cancel) {}

void ut_x11_randr_extension_list_output_properties(
    UtObject *object, uint32_t output,
    UtX11ClientRandrListOutputPropertiesCallback callback, void *user_data,
    UtObject *cancel) {}

void ut_x11_randr_extension_query_output_property(
    UtObject *object, uint32_t output, uint32_t property,
    UtX11ClientRandrQueryOutputPropertyCallback callback, void *user_data,
    UtObject *cancel) {}

void ut_x11_randr_extension_configure_output_property(UtObject *object,
                                                      uint32_t output,
                                                      uint32_t property,
                                                      bool pending, bool range,
                                                      UtObject *valid_values) {}

void ut_x11_randr_extension_change_output_property(UtObject *object,
                                                   uint32_t output,
                                                   uint32_t property,
                                                   uint32_t format, int mode) {}

void ut_x11_randr_extension_delete_output_property(UtObject *object,
                                                   uint32_t output,
                                                   uint32_t property) {}

void ut_x11_randr_extension_get_output_property(
    UtObject *object, uint32_t output, uint32_t property, uint32_t type,
    UtX11ClientRandrGetOutputPropertyCallback callback, void *user_data,
    UtObject *cancel) {}

void ut_x11_randr_extension_get_create_mode(
    UtObject *object, uint32_t window, int mode, const char *mode_name,
    UtX11ClientRandrCreateModeCallback callback, void *user_data,
    UtObject *cancel) {}

void ut_x11_randr_extension_destroy_mode(UtObject *object, uint32_t mode) {}

void ut_x11_randr_extension_add_output_mode(UtObject *object, uint32_t output,
                                            uint32_t mode) {}

void ut_x11_randr_extension_delete_output_mode(UtObject *object,
                                               uint32_t output, uint32_t mode) {
}

void ut_x11_randr_extension_get_crtc_info(
    UtObject *object, uint32_t crtc, uint32_t config_timestamp,
    UtX11ClientRandrGetCrtcInfoCallback callback, void *user_data,
    UtObject *cancel) {}

void ut_x11_randr_extension_set_crtc_config(
    UtObject *object, uint32_t crtc, uint32_t timestamp,
    uint32_t config_timestamp, int16_t x, int16_t y, uint32_t mode,
    UtX11ClientRandrSetCrtcConfigCallback callback, void *user_data,
    UtObject *cancel) {}

void ut_x11_randr_extension_get_crtc_gamma_size(
    UtObject *object, uint32_t crtc,
    UtX11ClientRandrGetCrtcGammaSizeCallback callback, void *user_data,
    UtObject *cancel) {}

void ut_x11_randr_extension_get_crtc_gamma(
    UtObject *object, uint32_t crtc,
    UtX11ClientRandrGetCrtcGammaCallback callback, void *user_data,
    UtObject *cancel) {}

void ut_x11_randr_extension_set_crtc_gamma(UtObject *object, uint32_t crtc,
                                           int red, int green, int blue) {}

void ut_x11_randr_extension_get_monitors(
    UtObject *object, uint32_t window,
    UtX11ClientRandrGetMonitorsCallback callback, void *user_data,
    UtObject *cancel) {
  assert(ut_object_is_x11_randr_extension(object));
  UtX11RandrExtension *self = (UtX11RandrExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);

  // ut_x11_client_send_request_with_reply(
  //     self->client, self->major_opcode, 42, request, get_monitors_reply_cb,
  //     get_monitors_error_cb, callback_data_new(callback, user_data), cancel);
}

void ut_x11_randr_extension_set_monitor(UtObject *object, uint32_t window,
                                        UtObject *monitor_info) {
  assert(ut_object_is_x11_randr_extension(object));
  UtX11RandrExtension *self = (UtX11RandrExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);
  // FIXME: monitor_info

  ut_x11_client_send_request(self->client, self->major_opcode, 43, request);
}

void ut_x11_randr_extension_delete_monitor(UtObject *object, uint32_t window,
                                           uint32_t name) {
  assert(ut_object_is_x11_randr_extension(object));
  UtX11RandrExtension *self = (UtX11RandrExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);
  ut_x11_buffer_append_card32(request, name);

  ut_x11_client_send_request(self->client, self->major_opcode, 44, request);
}

bool ut_object_is_x11_randr_extension(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
