#include <assert.h>

#include "ut-x11-buffer.h"
#include "ut-x11-client-private.h"
#include "ut-x11-extension.h"
#include "ut-x11-present-extension.h"
#include "ut.h"

typedef struct {
  UtObject object;
  void *callback;
  void *user_data;
} CallbackData;

static UtObjectInterface callback_data_object_interface = {
    .type_name = "PresentCallbackData", .interfaces = {{NULL, NULL}}};

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
  const UtX11EventCallbacks *event_callbacks;
  void *user_data;
  UtObject *cancel;
} UtX11PresentExtension;

static void query_version_reply_cb(UtObject *object, uint8_t data0,
                                   UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  uint32_t major_version = ut_x11_buffer_get_card32(data, &offset);
  uint32_t minor_version = ut_x11_buffer_get_card32(data, &offset);

  if (callback_data->callback != NULL) {
    UtX11ClientPresentQueryVersionCallback callback =
        (UtX11ClientPresentQueryVersionCallback)callback_data->callback;
    callback(callback_data->user_data, major_version, minor_version, NULL);
  }
}

static void query_version_error_cb(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback != NULL) {
    UtX11ClientPresentQueryVersionCallback callback =
        (UtX11ClientPresentQueryVersionCallback)callback_data->callback;
    callback(callback_data->user_data, 0, 0, error);
  }
}

static void decode_configure_notify(UtX11PresentExtension *self,
                                    UtObject *data) {
  size_t offset = 0;
  ut_x11_buffer_get_padding(data, &offset, 2);
  uint32_t event_id = ut_x11_buffer_get_card32(data, &offset);
  uint32_t window = ut_x11_buffer_get_card32(data, &offset);
  int16_t x = ut_x11_buffer_get_int16(data, &offset);
  int16_t y = ut_x11_buffer_get_int16(data, &offset);
  uint16_t width = ut_x11_buffer_get_card16(data, &offset);
  uint16_t height = ut_x11_buffer_get_card16(data, &offset);
  int16_t off_x = ut_x11_buffer_get_int16(data, &offset);
  int16_t off_y = ut_x11_buffer_get_int16(data, &offset);
  uint16_t pixmap_width = ut_x11_buffer_get_card16(data, &offset);
  uint16_t pixmap_height = ut_x11_buffer_get_card16(data, &offset);
  uint32_t pixmap_flags = ut_x11_buffer_get_card16(data, &offset);

  if (self->event_callbacks->present_configure_notify != NULL &&
      !ut_cancel_is_active(self->cancel)) {
    self->event_callbacks->present_configure_notify(
        self->user_data, event_id, window, x, y, width, height, off_x, off_y,
        pixmap_width, pixmap_height, pixmap_flags);
  }
}

static void decode_complete_notify(UtX11PresentExtension *self,
                                   UtObject *data) {
  size_t offset = 0;
  uint8_t kind = ut_x11_buffer_get_card8(data, &offset);
  uint8_t mode = ut_x11_buffer_get_card8(data, &offset);
  uint32_t event_id = ut_x11_buffer_get_card32(data, &offset);
  uint32_t window = ut_x11_buffer_get_card32(data, &offset);
  uint32_t serial = ut_x11_buffer_get_card32(data, &offset);
  uint64_t ust = ut_x11_buffer_get_card64(data, &offset);
  uint64_t msc = ut_x11_buffer_get_card64(data, &offset);

  if (self->event_callbacks->present_complete_notify != NULL &&
      !ut_cancel_is_active(self->cancel)) {
    self->event_callbacks->present_complete_notify(
        self->user_data, kind, mode, event_id, window, serial, ust, msc);
  }
}

static void decode_idle_notify(UtX11PresentExtension *self, UtObject *data) {
  size_t offset = 0;
  ut_x11_buffer_get_padding(data, &offset, 2);
  uint32_t event_id = ut_x11_buffer_get_card32(data, &offset);
  uint32_t window = ut_x11_buffer_get_card32(data, &offset);
  uint32_t serial = ut_x11_buffer_get_card32(data, &offset);
  uint32_t pixmap = ut_x11_buffer_get_card32(data, &offset);
  uint32_t idle_fence = ut_x11_buffer_get_card32(data, &offset);

  if (self->event_callbacks->present_idle_notify != NULL &&
      !ut_cancel_is_active(self->cancel)) {
    self->event_callbacks->present_idle_notify(
        self->user_data, event_id, window, serial, pixmap, idle_fence);
  }
}

static void ut_x11_present_extension_cleanup(UtObject *object) {
  UtX11PresentExtension *self = (UtX11PresentExtension *)object;
  ut_object_unref(self->cancel);
}

static uint8_t ut_x11_present_extension_get_major_opcode(UtObject *object) {
  UtX11PresentExtension *self = (UtX11PresentExtension *)object;
  return self->major_opcode;
}

static bool ut_x11_present_extension_decode_generic_event(UtObject *object,
                                                          uint16_t code,
                                                          UtObject *data) {
  UtX11PresentExtension *self = (UtX11PresentExtension *)object;

  switch (code) {
  case 0:
    decode_configure_notify(self, data);
    return true;
  case 1:
    decode_complete_notify(self, data);
    return true;
  case 2:
    decode_idle_notify(self, data);
    return true;
  default:
    return false;
  }
}

static UtObject *ut_x11_present_extension_decode_error(UtObject *object,
                                                       UtObject *data) {
  // FIXME
  return NULL;
}

static void ut_x11_present_extension_close(UtObject *object) {
  UtX11PresentExtension *self = (UtX11PresentExtension *)object;
  self->client = NULL;
}

static UtX11ExtensionInterface x11_extension_interface = {
    .get_major_opcode = ut_x11_present_extension_get_major_opcode,
    .decode_generic_event = ut_x11_present_extension_decode_generic_event,
    .decode_error = ut_x11_present_extension_decode_error,
    .close = ut_x11_present_extension_close};

static UtObjectInterface object_interface = {
    .type_name = "UtX11PresentExtension",
    .cleanup = ut_x11_present_extension_cleanup,
    .interfaces = {{&ut_x11_extension_id, &x11_extension_interface},
                   {NULL, NULL}}};

UtObject *
ut_x11_present_extension_new(UtObject *client, uint8_t major_opcode,
                             const UtX11EventCallbacks *event_callbacks,
                             void *user_data, UtObject *cancel) {
  UtObject *object =
      ut_object_new(sizeof(UtX11PresentExtension), &object_interface);
  UtX11PresentExtension *self = (UtX11PresentExtension *)object;
  self->client = client;
  self->major_opcode = major_opcode;
  self->event_callbacks = event_callbacks;
  self->user_data = user_data;
  self->cancel = ut_object_ref(cancel);
  return object;
}

void ut_x11_present_extension_query_version(
    UtObject *object, UtX11ClientPresentQueryVersionCallback callback,
    void *user_data, UtObject *cancel) {
  assert(ut_object_is_x11_present_extension(object));
  UtX11PresentExtension *self = (UtX11PresentExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, 1);
  ut_x11_buffer_append_card32(request, 0);

  ut_x11_client_send_request_with_reply(
      self->client, self->major_opcode, 0, request, query_version_reply_cb,
      query_version_error_cb, callback_data_new(callback, user_data), cancel);
}

void ut_x11_present_extension_pixmap(UtObject *object, uint32_t window,
                                     uint32_t pixmap, uint32_t serial,
                                     uint32_t valid_area, uint32_t update_area,
                                     int16_t x_off, int16_t y_off,
                                     uint32_t target_crtc, uint32_t wait_fence,
                                     uint32_t idle_fence, uint32_t options,
                                     uint64_t target_msc, uint64_t divisor,
                                     uint64_t remainder) {
  assert(ut_object_is_x11_present_extension(object));
  UtX11PresentExtension *self = (UtX11PresentExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);
  ut_x11_buffer_append_card32(request, pixmap);
  ut_x11_buffer_append_card32(request, serial);
  ut_x11_buffer_append_card32(request, valid_area);
  ut_x11_buffer_append_card32(request, update_area);
  ut_x11_buffer_append_int16(request, x_off);
  ut_x11_buffer_append_int16(request, y_off);
  ut_x11_buffer_append_card32(request, target_crtc);
  ut_x11_buffer_append_card32(request, wait_fence);
  ut_x11_buffer_append_card32(request, idle_fence);
  ut_x11_buffer_append_card32(request, options);
  ut_x11_buffer_append_padding(request, 4);
  ut_x11_buffer_append_card64(request, target_msc);
  ut_x11_buffer_append_card32(request, divisor);
  ut_x11_buffer_append_card32(request, remainder);
  // FIXME: notifies

  ut_x11_client_send_request(self->client, self->major_opcode, 1, request);
}

void ut_x11_present_extension_notify_msc(UtObject *object, uint32_t window,
                                         uint32_t serial, uint64_t target_msc,
                                         uint64_t divisor, uint64_t remainder) {
  assert(ut_object_is_x11_present_extension(object));
  UtX11PresentExtension *self = (UtX11PresentExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);
  ut_x11_buffer_append_card32(request, serial);
  ut_x11_buffer_append_padding(request, 4);
  ut_x11_buffer_append_card64(request, target_msc);
  ut_x11_buffer_append_card32(request, divisor);
  ut_x11_buffer_append_card32(request, remainder);

  ut_x11_client_send_request(self->client, self->major_opcode, 2, request);
}

uint32_t ut_x11_present_extension_select_input(UtObject *object,
                                               uint32_t window,
                                               uint32_t event_mask) {
  assert(ut_object_is_x11_present_extension(object));
  UtX11PresentExtension *self = (UtX11PresentExtension *)object;

  uint32_t event_id = ut_x11_client_create_resource_id(self->client);
  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, event_id);
  ut_x11_buffer_append_card32(request, window);
  ut_x11_buffer_append_card32(request, event_mask);

  ut_x11_client_send_request(self->client, self->major_opcode, 3, request);

  return event_id;
}

void ut_x11_present_extension_query_capabilities(UtObject *object,
                                                 uint32_t crtc_or_window) {
  assert(ut_object_is_x11_present_extension(object));
  UtX11PresentExtension *self = (UtX11PresentExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, crtc_or_window);

  ut_x11_client_send_request(self->client, self->major_opcode, 4, request);
}

bool ut_object_is_x11_present_extension(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
