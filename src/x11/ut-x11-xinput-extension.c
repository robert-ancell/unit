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

static UtObject *decode_key_class(uint16_t source_id, UtObject *data) {
  size_t offset = 0;
  size_t keys_length = ut_x11_buffer_get_card16(data, &offset);
  UtObjectRef keys = ut_uint32_list_new();
  for (size_t i = 0; i < keys_length; i++) {
    ut_uint32_list_append(keys, ut_x11_buffer_get_card32(data, &offset));
  }
  return ut_x11_key_class_new(keys);
}

static UtObject *decode_button_class(uint16_t source_id, UtObject *data) {
  size_t offset = 0;
  size_t buttons_length = ut_x11_buffer_get_card16(data, &offset);
  size_t state_length = (buttons_length + 31) / 32;
  UtObjectRef state = ut_uint32_list_new();
  for (size_t i = 0; i < state_length; i++) {
    ut_uint32_list_append(state, ut_x11_buffer_get_card32(data, &offset));
  }
  UtObjectRef labels = ut_uint32_list_new();
  for (size_t i = 0; i < buttons_length; i++) {
    ut_uint32_list_append(labels, ut_x11_buffer_get_card32(data, &offset));
  }
  return ut_x11_button_class_new(state, labels);
}

static UtObject *decode_valuator_class(uint16_t source_id, UtObject *data) {
  size_t offset = 0;
  uint16_t number = ut_x11_buffer_get_card16(data, &offset);
  uint32_t label = ut_x11_buffer_get_card32(data, &offset);
  double min = ut_x11_buffer_get_fp3232(data, &offset);
  double max = ut_x11_buffer_get_fp3232(data, &offset);
  double value = ut_x11_buffer_get_fp3232(data, &offset);
  uint32_t resolution = ut_x11_buffer_get_card32(data, &offset);
  uint8_t mode = ut_x11_buffer_get_card8(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 3);
  return ut_x11_valuator_class_new(number, label, min, max, value, resolution,
                                   mode);
}

static UtObject *decode_scroll_class(uint16_t source_id, UtObject *data) {
  size_t offset = 0;
  uint16_t number = ut_x11_buffer_get_card16(data, &offset);
  uint16_t scroll_type = ut_x11_buffer_get_card16(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 2);
  uint32_t flags = ut_x11_buffer_get_card32(data, &offset);
  double increment = ut_x11_buffer_get_fp3232(data, &offset);
  return ut_x11_scroll_class_new(number, scroll_type, flags, increment);
}

static UtObject *decode_touch_class(uint16_t source_id, UtObject *data) {
  size_t offset = 0;
  uint8_t mode = ut_x11_buffer_get_card8(data, &offset);
  uint8_t num_touches = ut_x11_buffer_get_card8(data, &offset);
  return ut_x11_touch_class_new(mode, num_touches);
}

static UtObject *decode_device_classes(UtObject *data, size_t *offset,
                                       size_t classes_length) {
  UtObjectRef classes = ut_object_list_new();
  for (size_t j = 0; j < classes_length; j++) {
    uint16_t type = ut_x11_buffer_get_card16(data, offset);
    size_t length = ut_x11_buffer_get_card16(data, offset);
    uint16_t source_id = ut_x11_buffer_get_card16(data, offset);
    UtObjectRef class_data =
        ut_x11_buffer_get_sub_buffer(data, offset, (length * 4) - 6);
    UtObjectRef class = NULL;
    switch (type) {
    case 0:
      class = decode_key_class(source_id, class_data);
      break;
    case 1:
      class = decode_button_class(source_id, class_data);
      break;
    case 2:
      class = decode_valuator_class(source_id, class_data);
      break;
    case 3:
      class = decode_scroll_class(source_id, class_data);
      break;
    case 8:
      class = decode_touch_class(source_id, class_data);
      break;
    default:
      class = ut_x11_unknown_input_class_new(type);
      break;
    }
  }

  return ut_object_ref(classes);
}

static void decode_device_changed(UtX11XinputExtension *self, UtObject *data) {
  size_t offset = 0;
  uint16_t device_id = ut_x11_buffer_get_card16(data, &offset);
  uint32_t timestamp = ut_x11_buffer_get_card32(data, &offset);
  uint16_t classes_length = ut_x11_buffer_get_card16(data, &offset);
  /*uint16_t source_id = */ ut_x11_buffer_get_card16(data, &offset);
  UtX11DeviceChangeReason reason = ut_x11_buffer_get_card8(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 11);
  UtObjectRef classes = decode_device_classes(data, &offset, classes_length);

  if (self->event_callbacks->input_device_changed != NULL &&
      !ut_cancel_is_active(self->cancel)) {
    self->event_callbacks->input_device_changed(self->user_data, device_id,
                                                timestamp, reason, classes);
  }
}

static void decode_modifier_info(UtObject *data, size_t *offset) {
  /*uint32_t base = */ ut_x11_buffer_get_card32(data, offset);
  /*uint32_t latched = */ ut_x11_buffer_get_card32(data, offset);
  /*uint32_t locked = */ ut_x11_buffer_get_card32(data, offset);
  /*uint32_t effective = */ ut_x11_buffer_get_card32(data, offset);
}

static void decode_group_info(UtObject *data, size_t *offset) {
  /*uint8_t base = */ ut_x11_buffer_get_card8(data, offset);
  /*uint8_t latched = */ ut_x11_buffer_get_card8(data, offset);
  /*uint8_t locked = */ ut_x11_buffer_get_card8(data, offset);
  /*uint8_t effective = */ ut_x11_buffer_get_card8(data, offset);
}

static void decode_key_event(UtObject *data, uint16_t *device_id,
                             uint32_t *timestamp, uint32_t *detail,
                             uint32_t *event, double *event_x, double *event_y,
                             UtX11KeyEventFlag *flags, UtObject **button_mask,
                             UtObject **valuator_mask) {
  size_t offset = 0;
  *device_id = ut_x11_buffer_get_card16(data, &offset);
  *timestamp = ut_x11_buffer_get_card32(data, &offset);
  *detail = ut_x11_buffer_get_card32(data, &offset);
  /*uint32_t root = */ ut_x11_buffer_get_card32(data, &offset);
  *event = ut_x11_buffer_get_card32(data, &offset);
  /*uint32_t child = */ ut_x11_buffer_get_card32(data, &offset);
  /*double root_x = */ ut_x11_buffer_get_fp1616(data, &offset);
  /*double root_y = */ ut_x11_buffer_get_fp1616(data, &offset);
  *event_x = ut_x11_buffer_get_fp1616(data, &offset);
  *event_y = ut_x11_buffer_get_fp1616(data, &offset);
  size_t buttons_length = ut_x11_buffer_get_card16(data, &offset);
  size_t valuators_length = ut_x11_buffer_get_card16(data, &offset);
  /*uint16_t source_id = */ ut_x11_buffer_get_card16(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 2);
  *flags = ut_x11_buffer_get_card32(data, &offset);
  decode_modifier_info(data, &offset);
  decode_group_info(data, &offset);
  *button_mask = ut_uint32_list_new();
  for (size_t i = 0; i < buttons_length; i++) {
    ut_uint32_list_append(*button_mask,
                          ut_x11_buffer_get_card32(data, &offset));
  }
  *valuator_mask = ut_uint32_list_new();
  for (size_t i = 0; i < valuators_length; i++) {
    ut_uint32_list_append(*valuator_mask,
                          ut_x11_buffer_get_card32(data, &offset));
  }
  // FIXME axisvalues
}

static void decode_key_press(UtX11XinputExtension *self, UtObject *data) {
  uint16_t device_id;
  uint32_t timestamp, detail, event;
  double event_x, event_y;
  UtX11KeyEventFlag flags;
  UtObjectRef button_mask = NULL;
  UtObjectRef valuator_mask = NULL;
  decode_key_event(data, &device_id, &timestamp, &detail, &event, &event_x,
                   &event_y, &flags, &button_mask, &valuator_mask);

  if (self->event_callbacks->input_key_press != NULL &&
      !ut_cancel_is_active(self->cancel)) {
    self->event_callbacks->input_key_press(self->user_data, device_id,
                                           timestamp, event, detail, event_x,
                                           event_y, flags);
  }
}

static void decode_key_release(UtX11XinputExtension *self, UtObject *data) {
  uint16_t device_id;
  uint32_t timestamp, detail, event;
  double event_x, event_y;
  UtX11KeyEventFlag flags;
  UtObjectRef button_mask = NULL;
  UtObjectRef valuator_mask = NULL;
  decode_key_event(data, &device_id, &timestamp, &detail, &event, &event_x,
                   &event_y, &flags, &button_mask, &valuator_mask);

  if (self->event_callbacks->input_key_release != NULL &&
      !ut_cancel_is_active(self->cancel)) {
    self->event_callbacks->input_key_release(self->user_data, device_id,
                                             timestamp, event, detail, event_x,
                                             event_y, flags);
  }
}

static void decode_pointer_event(UtObject *data, uint16_t *device_id,
                                 uint32_t *timestamp, uint32_t *detail,
                                 uint32_t *event, double *event_x,
                                 double *event_y, UtX11PointerEventFlag *flags,
                                 UtObject **button_mask,
                                 UtObject **valuator_mask) {
  size_t offset = 0;
  *device_id = ut_x11_buffer_get_card16(data, &offset);
  *timestamp = ut_x11_buffer_get_card32(data, &offset);
  *detail = ut_x11_buffer_get_card32(data, &offset);
  /*uint32_t root = */ ut_x11_buffer_get_card32(data, &offset);
  *event = ut_x11_buffer_get_card32(data, &offset);
  /*uint32_t child = */ ut_x11_buffer_get_card32(data, &offset);
  /*double root_x = */ ut_x11_buffer_get_fp1616(data, &offset);
  /*double root_y = */ ut_x11_buffer_get_fp1616(data, &offset);
  *event_x = ut_x11_buffer_get_fp1616(data, &offset);
  *event_y = ut_x11_buffer_get_fp1616(data, &offset);
  size_t buttons_length = ut_x11_buffer_get_card16(data, &offset);
  size_t valuators_length = ut_x11_buffer_get_card16(data, &offset);
  /*uint16_t source_id = */ ut_x11_buffer_get_card16(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 2);
  *flags = ut_x11_buffer_get_card32(data, &offset);
  decode_modifier_info(data, &offset);
  decode_group_info(data, &offset);
  *button_mask = ut_uint32_list_new();
  for (size_t i = 0; i < buttons_length; i++) {
    ut_uint32_list_append(*button_mask,
                          ut_x11_buffer_get_card32(data, &offset));
  }
  *valuator_mask = ut_uint32_list_new();
  for (size_t i = 0; i < valuators_length; i++) {
    ut_uint32_list_append(*valuator_mask,
                          ut_x11_buffer_get_card32(data, &offset));
  }
  // FIXME axisvalues
}

static void decode_button_press(UtX11XinputExtension *self, UtObject *data) {
  uint16_t device_id;
  uint32_t timestamp, detail, event;
  double event_x, event_y;
  UtX11PointerEventFlag flags;
  UtObjectRef button_mask = NULL;
  UtObjectRef valuator_mask = NULL;
  decode_pointer_event(data, &device_id, &timestamp, &detail, &event, &event_x,
                       &event_y, &flags, &button_mask, &valuator_mask);

  if (self->event_callbacks->input_button_press != NULL &&
      !ut_cancel_is_active(self->cancel)) {
    self->event_callbacks->input_button_press(self->user_data, device_id,
                                              timestamp, event, detail, event_x,
                                              event_y, flags);
  }
}

static void decode_button_release(UtX11XinputExtension *self, UtObject *data) {
  uint16_t device_id;
  uint32_t timestamp, detail, event;
  double event_x, event_y;
  UtX11PointerEventFlag flags;
  UtObjectRef button_mask = NULL;
  UtObjectRef valuator_mask = NULL;
  decode_pointer_event(data, &device_id, &timestamp, &detail, &event, &event_x,
                       &event_y, &flags, &button_mask, &valuator_mask);

  if (self->event_callbacks->input_button_release != NULL &&
      !ut_cancel_is_active(self->cancel)) {
    self->event_callbacks->input_button_release(self->user_data, device_id,
                                                timestamp, event, detail,
                                                event_x, event_y, flags);
  }
}

static void decode_motion(UtX11XinputExtension *self, UtObject *data) {
  uint16_t device_id;
  uint32_t timestamp, detail, event;
  double event_x, event_y;
  UtX11PointerEventFlag flags;
  UtObjectRef button_mask = NULL;
  UtObjectRef valuator_mask = NULL;
  decode_pointer_event(data, &device_id, &timestamp, &detail, &event, &event_x,
                       &event_y, &flags, &button_mask, &valuator_mask);

  if (self->event_callbacks->input_motion != NULL &&
      !ut_cancel_is_active(self->cancel)) {
    self->event_callbacks->input_motion(self->user_data, device_id, timestamp,
                                        event, event_x, event_y, flags);
  }
}

static void decode_notify_event(UtObject *data, uint16_t *device_id,
                                uint32_t *timestamp, UtX11InputNotifyMode *mode,
                                UtX11InputNotifyDetail *detail, uint32_t *event,
                                double *event_x, double *event_y,
                                UtObject **buttons) {
  size_t offset = 0;
  *device_id = ut_x11_buffer_get_card16(data, &offset);
  *timestamp = ut_x11_buffer_get_card32(data, &offset);
  /*uint16_t source_id = */ ut_x11_buffer_get_card16(data, &offset);
  *mode = ut_x11_buffer_get_card8(data, &offset);
  *detail = ut_x11_buffer_get_card8(data, &offset);
  /*uint32_t root = */ ut_x11_buffer_get_card32(data, &offset);
  *event = ut_x11_buffer_get_card32(data, &offset);
  /*uint32_t child = */ ut_x11_buffer_get_card32(data, &offset);
  /*double root_x = */ ut_x11_buffer_get_fp1616(data, &offset);
  /*double root_y = */ ut_x11_buffer_get_fp1616(data, &offset);
  *event_x = ut_x11_buffer_get_fp1616(data, &offset);
  *event_y = ut_x11_buffer_get_fp1616(data, &offset);
  /*bool same_screen = */ ut_x11_buffer_get_bool(data, &offset);
  /*bool focus = */ ut_x11_buffer_get_bool(data, &offset);
  size_t buttons_length = ut_x11_buffer_get_card16(data, &offset);
  decode_modifier_info(data, &offset);
  decode_group_info(data, &offset);
  *buttons = ut_uint32_list_new();
  for (size_t i = 0; i < buttons_length; i++) {
    ut_uint32_list_append(*buttons, ut_x11_buffer_get_card32(data, &offset));
  }
}

static void decode_enter(UtX11XinputExtension *self, UtObject *data) {
  uint16_t device_id;
  uint32_t timestamp, event;
  UtX11InputNotifyMode mode;
  UtX11InputNotifyDetail detail;
  double event_x, event_y;
  UtObjectRef buttons = NULL;
  decode_notify_event(data, &device_id, &timestamp, &mode, &detail, &event,
                      &event_x, &event_y, &buttons);

  if (self->event_callbacks->input_enter != NULL &&
      !ut_cancel_is_active(self->cancel)) {
    self->event_callbacks->input_enter(self->user_data, device_id, timestamp,
                                       mode, detail, event, event_x, event_y);
  }
}

static void decode_leave(UtX11XinputExtension *self, UtObject *data) {
  uint16_t device_id;
  uint32_t timestamp, event;
  UtX11InputNotifyMode mode;
  UtX11InputNotifyDetail detail;
  double event_x, event_y;
  UtObjectRef buttons = NULL;
  decode_notify_event(data, &device_id, &timestamp, &mode, &detail, &event,
                      &event_x, &event_y, &buttons);

  if (self->event_callbacks->input_leave != NULL &&
      !ut_cancel_is_active(self->cancel)) {
    self->event_callbacks->input_leave(self->user_data, device_id, timestamp,
                                       mode, detail, event, event_x, event_y);
  }
}

static void decode_focus_in(UtX11XinputExtension *self, UtObject *data) {
  uint16_t device_id;
  uint32_t timestamp, event;
  UtX11InputNotifyMode mode;
  UtX11InputNotifyDetail detail;
  double event_x, event_y;
  UtObjectRef buttons = NULL;
  decode_notify_event(data, &device_id, &timestamp, &mode, &detail, &event,
                      &event_x, &event_y, &buttons);

  if (self->event_callbacks->input_focus_in != NULL &&
      !ut_cancel_is_active(self->cancel)) {
    self->event_callbacks->input_focus_in(self->user_data, event, timestamp,
                                          mode, detail, device_id);
  }
}

static void decode_focus_out(UtX11XinputExtension *self, UtObject *data) {
  uint16_t device_id;
  uint32_t timestamp, event;
  UtX11InputNotifyMode mode;
  UtX11InputNotifyDetail detail;
  double event_x, event_y;
  UtObjectRef buttons = NULL;
  decode_notify_event(data, &device_id, &timestamp, &mode, &detail, &event,
                      &event_x, &event_y, &buttons);

  if (self->event_callbacks->input_focus_out != NULL &&
      !ut_cancel_is_active(self->cancel)) {
    self->event_callbacks->input_focus_out(self->user_data, event, timestamp,
                                           mode, detail, device_id);
  }
}

static void decode_touch_event(UtObject *data, uint16_t *device_id,
                               uint32_t *timestamp, uint32_t *detail,
                               uint32_t *event, double *event_x,
                               double *event_y, UtX11TouchEventFlag *flags,
                               UtObject **button_mask,
                               UtObject **valuator_mask) {
  size_t offset = 0;
  *device_id = ut_x11_buffer_get_card16(data, &offset);
  *timestamp = ut_x11_buffer_get_card32(data, &offset);
  *detail = ut_x11_buffer_get_card32(data, &offset);
  /*uint32_t root = */ ut_x11_buffer_get_card32(data, &offset);
  *event = ut_x11_buffer_get_card32(data, &offset);
  /*uint32_t child = */ ut_x11_buffer_get_card32(data, &offset);
  /*double root_x = */ ut_x11_buffer_get_fp1616(data, &offset);
  /*double root_y = */ ut_x11_buffer_get_fp1616(data, &offset);
  *event_x = ut_x11_buffer_get_fp1616(data, &offset);
  *event_y = ut_x11_buffer_get_fp1616(data, &offset);
  size_t buttons_length = ut_x11_buffer_get_card16(data, &offset);
  size_t valuators_length = ut_x11_buffer_get_card16(data, &offset);
  /*uint16_t source_id = */ ut_x11_buffer_get_card16(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 2);
  *flags = ut_x11_buffer_get_card32(data, &offset);
  decode_modifier_info(data, &offset);
  decode_group_info(data, &offset);
  *button_mask = ut_uint32_list_new();
  for (size_t i = 0; i < buttons_length; i++) {
    ut_uint32_list_append(*button_mask,
                          ut_x11_buffer_get_card32(data, &offset));
  }
  *valuator_mask = ut_uint32_list_new();
  for (size_t i = 0; i < valuators_length; i++) {
    ut_uint32_list_append(*valuator_mask,
                          ut_x11_buffer_get_card32(data, &offset));
  }
  // FIXME axisvalues
}

static void decode_touch_begin(UtX11XinputExtension *self, UtObject *data) {
  uint16_t device_id;
  uint32_t timestamp, detail, event;
  UtX11TouchEventFlag flags;
  double event_x, event_y;
  UtObjectRef button_mask = NULL;
  UtObjectRef valuator_mask = NULL;
  decode_touch_event(data, &device_id, &timestamp, &detail, &event, &event_x,
                     &event_y, &flags, &button_mask, &valuator_mask);

  if (self->event_callbacks->input_touch_begin != NULL &&
      !ut_cancel_is_active(self->cancel)) {
    self->event_callbacks->input_touch_begin(
        self->user_data, device_id, timestamp, event, detail, event_x, event_y);
  }
}

static void decode_touch_update(UtX11XinputExtension *self, UtObject *data) {
  uint16_t device_id;
  uint32_t timestamp, detail, event;
  UtX11TouchEventFlag flags;
  double event_x, event_y;
  UtObjectRef button_mask = NULL;
  UtObjectRef valuator_mask = NULL;
  decode_touch_event(data, &device_id, &timestamp, &detail, &event, &event_x,
                     &event_y, &flags, &button_mask, &valuator_mask);

  if (self->event_callbacks->input_touch_update != NULL &&
      !ut_cancel_is_active(self->cancel)) {
    self->event_callbacks->input_touch_update(
        self->user_data, device_id, timestamp, event, detail, event_x, event_y);
  }
}

static void decode_touch_end(UtX11XinputExtension *self, UtObject *data) {
  uint16_t device_id;
  uint32_t timestamp, detail, event;
  UtX11TouchEventFlag flags;
  double event_x, event_y;
  UtObjectRef button_mask = NULL;
  UtObjectRef valuator_mask = NULL;
  decode_touch_event(data, &device_id, &timestamp, &detail, &event, &event_x,
                     &event_y, &flags, &button_mask, &valuator_mask);

  if (self->event_callbacks->input_touch_end != NULL &&
      !ut_cancel_is_active(self->cancel)) {
    self->event_callbacks->input_touch_end(
        self->user_data, device_id, timestamp, event, detail, event_x, event_y);
  }
}

static void query_version_reply_cb(UtObject *object, uint8_t data0,
                                   UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  ut_x11_buffer_get_padding(data, &offset, 1);
  uint16_t major_version = ut_x11_buffer_get_card16(data, &offset);
  uint16_t minor_version = ut_x11_buffer_get_card16(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 20);

  if (callback_data->callback != NULL) {
    UtX11XinputQueryVersionCallback callback =
        (UtX11XinputQueryVersionCallback)callback_data->callback;
    callback(callback_data->user_data, major_version, minor_version, NULL);
  }
}

static void query_version_error_cb(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback != NULL) {
    UtX11XinputQueryVersionCallback callback =
        (UtX11XinputQueryVersionCallback)callback_data->callback;
    callback(callback_data->user_data, 0, 0, error);
  }
}

static void query_device_reply_cb(UtObject *object, uint8_t data0,
                                  UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  uint16_t infos_length = ut_x11_buffer_get_card16(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 22);
  UtObjectRef infos = ut_object_list_new();
  for (size_t i = 0; i < infos_length; i++) {
    uint16_t device_id = ut_x11_buffer_get_card16(data, &offset);
    uint16_t type = ut_x11_buffer_get_card16(data, &offset);
    uint16_t attachment = ut_x11_buffer_get_card16(data, &offset);
    uint16_t classes_length = ut_x11_buffer_get_card16(data, &offset);
    uint16_t name_length = ut_x11_buffer_get_card16(data, &offset);
    bool enabled = ut_x11_buffer_get_bool(data, &offset);
    ut_x11_buffer_get_padding(data, &offset, 1);
    ut_cstring_ref name = ut_x11_buffer_get_string8(data, &offset, name_length);
    ut_x11_buffer_get_align_padding(data, &offset, 4);
    UtObjectRef classes = decode_device_classes(data, &offset, classes_length);

    UtObjectRef info = ut_x11_input_device_info_new(
        device_id, name, type, attachment, classes, enabled);
    ut_list_append(infos, info);
  }

  if (callback_data->callback != NULL) {
    UtX11QueryDeviceCallback callback =
        (UtX11QueryDeviceCallback)callback_data->callback;
    callback(callback_data->user_data, infos, NULL);
  }
}

static void query_device_error_cb(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback != NULL) {
    UtX11QueryDeviceCallback callback =
        (UtX11QueryDeviceCallback)callback_data->callback;
    callback(callback_data->user_data, NULL, error);
  }
}

static void get_focus_reply_cb(UtObject *object, uint8_t data0,
                               UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  uint32_t window = ut_x11_buffer_get_card32(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 20);

  if (callback_data->callback != NULL) {
    UtX11GetFocusCallback callback =
        (UtX11GetFocusCallback)callback_data->callback;
    callback(callback_data->user_data, window, NULL);
  }
}

static void get_focus_error_cb(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback != NULL) {
    UtX11GetFocusCallback callback =
        (UtX11GetFocusCallback)callback_data->callback;
    callback(callback_data->user_data, 0, error);
  }
}

static void grab_device_reply_cb(UtObject *object, uint8_t data0,
                                 UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  uint8_t status = ut_x11_buffer_get_card8(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 23);

  if (callback_data->callback != NULL) {
    UtX11GrabDeviceCallback callback =
        (UtX11GrabDeviceCallback)callback_data->callback;
    callback(callback_data->user_data, status, NULL);
  }
}

static void grab_device_error_cb(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback != NULL) {
    UtX11GrabDeviceCallback callback =
        (UtX11GrabDeviceCallback)callback_data->callback;
    callback(callback_data->user_data, 0, error);
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

static uint8_t ut_x11_xinput_extension_get_first_event(UtObject *object) {
  UtX11XinputExtension *self = (UtX11XinputExtension *)object;
  return self->first_event;
}

static uint8_t ut_x11_xinput_extension_get_first_error(UtObject *object) {
  UtX11XinputExtension *self = (UtX11XinputExtension *)object;
  return self->first_error;
}

static bool ut_x11_xinput_extension_decode_generic_event(UtObject *object,
                                                         uint16_t code,
                                                         UtObject *data) {
  UtX11XinputExtension *self = (UtX11XinputExtension *)object;
  switch (code) {
  case 1:
    decode_device_changed(self, data);
    return true;
  case 2:
    decode_key_press(self, data);
    return true;
  case 3:
    decode_key_release(self, data);
    return true;
  case 4:
    decode_button_press(self, data);
    return true;
  case 5:
    decode_button_release(self, data);
    return true;
  case 6:
    decode_motion(self, data);
    return true;
  case 7:
    decode_enter(self, data);
    return true;
  case 8:
    decode_leave(self, data);
    return true;
  case 9:
    decode_focus_in(self, data);
    return true;
  case 10:
    decode_focus_out(self, data);
    return true;
  case 18:
    decode_touch_begin(self, data);
    return true;
  case 19:
    decode_touch_update(self, data);
    return true;
  case 20:
    decode_touch_end(self, data);
    return true;
  default:
    return false;
  }
}

static UtX11ErrorCode ut_x11_xinput_extension_decode_error(UtObject *object,
                                                           uint8_t code) {
  switch (code) {
  case 0:
    return UT_X11_ERROR_INPUT_DEVICE;
  case 1:
    return UT_X11_ERROR_INPUT_EVENT;
  case 2:
    return UT_X11_ERROR_INPUT_MODE;
  case 3:
    return UT_X11_ERROR_INPUT_DEVICE_BUSY;
  case 4:
    return UT_X11_ERROR_INPUT_CLASS;
  default:
    return UT_X11_ERROR_UNKNOWN;
  }
}

static void ut_x11_xinput_extension_close(UtObject *object) {
  UtX11XinputExtension *self = (UtX11XinputExtension *)object;
  self->client = NULL;
}

static UtX11ExtensionInterface x11_extension_interface = {
    .get_major_opcode = ut_x11_xinput_extension_get_major_opcode,
    .get_first_event = ut_x11_xinput_extension_get_first_event,
    .get_first_error = ut_x11_xinput_extension_get_first_error,
    .decode_generic_event = ut_x11_xinput_extension_decode_generic_event,
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

void ut_x11_xinput_extension_select_events(UtObject *object, uint32_t window,
                                           UtObject *masks) {
  assert(ut_object_is_x11_xinput_extension(object));
  UtX11XinputExtension *self = (UtX11XinputExtension *)object;

  size_t masks_length = ut_list_get_length(masks);

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);
  ut_x11_buffer_append_card16(request, masks_length);
  ut_x11_buffer_append_padding(request, 2);
  for (size_t i = 0; i < masks_length; i++) {
    UtObjectRef mask = ut_list_get_element(masks, i);
    ut_x11_buffer_append_card16(request,
                                ut_x11_input_event_mask_get_device_id(mask));
    ut_x11_buffer_append_card16(request, 2);
    uint64_t mask_value = ut_x11_input_event_mask_get_value(mask);
    ut_x11_buffer_append_card32(request, mask_value & 0xffffffff);
    ut_x11_buffer_append_card32(request, mask_value >> 32);
  }

  ut_x11_client_send_request(self->client, self->major_opcode, 46, request);
}

void ut_x11_xinput_extension_query_version(
    UtObject *object, UtX11XinputQueryVersionCallback callback, void *user_data,
    UtObject *cancel) {
  assert(ut_object_is_x11_xinput_extension(object));
  UtX11XinputExtension *self = (UtX11XinputExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card16(request, 2);
  ut_x11_buffer_append_card16(request, 4);

  ut_x11_client_send_request_with_reply(
      self->client, self->major_opcode, 47, request, query_version_reply_cb,
      query_version_error_cb, callback_data_new(callback, user_data), cancel);
}

void ut_x11_xinput_extension_query_device(UtObject *object, uint16_t device_id,
                                          UtX11QueryDeviceCallback callback,
                                          void *user_data, UtObject *cancel) {
  assert(ut_object_is_x11_xinput_extension(object));
  UtX11XinputExtension *self = (UtX11XinputExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card16(request, device_id);
  ut_x11_buffer_append_padding(request, 2);

  ut_x11_client_send_request_with_reply(
      self->client, self->major_opcode, 48, request, query_device_reply_cb,
      query_device_error_cb, callback_data_new(callback, user_data), cancel);
}

void ut_x11_xinput_extension_set_focus(UtObject *object, uint32_t window,
                                       uint32_t timestamp, uint16_t device_id) {
  assert(ut_object_is_x11_xinput_extension(object));
  UtX11XinputExtension *self = (UtX11XinputExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);
  ut_x11_buffer_append_card32(request, timestamp);
  ut_x11_buffer_append_card16(request, device_id);
  ut_x11_buffer_append_padding(request, 2);

  ut_x11_client_send_request(self->client, self->major_opcode, 49, request);
}

void ut_x11_xinput_extension_get_focus(UtObject *object, uint16_t device_id,
                                       UtX11GetFocusCallback callback,
                                       void *user_data, UtObject *cancel) {
  assert(ut_object_is_x11_xinput_extension(object));
  UtX11XinputExtension *self = (UtX11XinputExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card16(request, device_id);
  ut_x11_buffer_append_padding(request, 2);

  ut_x11_client_send_request_with_reply(
      self->client, self->major_opcode, 50, request, get_focus_reply_cb,
      get_focus_error_cb, callback_data_new(callback, user_data), cancel);
}

void ut_x11_xinput_extension_grab_device(UtObject *object, uint32_t window,
                                         uint32_t timestamp, uint32_t cursor,
                                         uint16_t device_id, uint8_t model,
                                         uint8_t paired_device_mode,
                                         bool owner_events, UtObject *masks,
                                         UtX11GrabDeviceCallback callback,
                                         void *user_data, UtObject *cancel) {
  assert(ut_object_is_x11_xinput_extension(object));
  UtX11XinputExtension *self = (UtX11XinputExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);
  ut_x11_buffer_append_card32(request, timestamp);
  ut_x11_buffer_append_card32(request, cursor);
  ut_x11_buffer_append_card16(request, device_id);
  ut_x11_buffer_append_card8(request, model);
  ut_x11_buffer_append_card8(request, paired_device_mode);
  ut_x11_buffer_append_bool(request, owner_events);
  ut_x11_buffer_append_padding(request, 1);
  size_t masks_length = masks != NULL ? ut_list_get_length(masks) : 0;
  ut_x11_buffer_append_card16(request, masks_length);
  for (size_t i = 0; i < masks_length; i++) {
    ut_x11_buffer_append_card32(request, ut_uint32_list_get_element(masks, i));
  }

  ut_x11_client_send_request_with_reply(
      self->client, self->major_opcode, 51, request, grab_device_reply_cb,
      grab_device_error_cb, callback_data_new(callback, user_data), cancel);
}

void ut_x11_xinput_extension_ungrab_device(UtObject *object,
                                           uint16_t device_id) {
  assert(ut_object_is_x11_xinput_extension(object));
  UtX11XinputExtension *self = (UtX11XinputExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card16(request, device_id);
  ut_x11_buffer_append_padding(request, 2);

  ut_x11_client_send_request(self->client, self->major_opcode, 52, request);
}

// PassiveGrabDevice

// PassiveUngrabDevice

// ListProperties

// ChangeProperties

// DeleteProperties

// GetProperty

// GetSelectedEvents

bool ut_object_is_x11_xinput_extension(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
