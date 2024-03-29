#include <assert.h>
#include <unistd.h>

#include "ut-x11-buffer.h"
#include "ut-x11-client-private.h"
#include "ut-x11-core.h"
#include "ut-x11-extension.h"
#include "ut.h"

typedef enum {
  WINDOW_CLASS_INHERIT_FROM_PARENT = 0,
  WINDOW_CLASS_INPUT_OUTPUT = 1,
  WINDOW_CLASS_INPUT_ONLY = 2
} UtX11WindowClass;

typedef enum {
  VALUE_MASK_BACKGROUND_PIXMAP = 0x00000001,
  VALUE_MASK_BACKGROUND_PIXEL = 0x00000002,
  VALUE_MASK_BORDER_PIXMAP = 0x00000004,
  VALUE_MASK_BORDER_PIXEL = 0x00000008,
  VALUE_MASK_BIT_GRAVITY = 0x00000010,
  VALUE_MASK_WIN_GRAVITY = 0x00000020,
  VALUE_MASK_BACKING_STORE = 0x00000040,
  VALUE_MASK_BACKING_PLANES = 0x00000080,
  VALUE_MASK_BACKING_PIXEL = 0x00000100,
  VALUE_MASK_OVERRIDE_REDIRECT = 0x00000200,
  VALUE_MASK_SAVE_UNDER = 0x00000400,
  VALUE_MASK_EVENT_MASK = 0x00000800,
  VALUE_MASK_DO_NOT_PROPAGATE_MASK = 0x00001000,
  VALUE_MASK_COLORMAP = 0x00002000,
  VALUE_MASK_CURSOR = 0x00004000
} UtX11ValueMask;

typedef struct _UtX11Core UtX11Core;

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
    .type_name = "CoreCallbackData", .cleanup = callback_data_cleanup};

static UtObject *callback_data_new(UtObject *callback_object, void *callback) {
  UtObject *object =
      ut_object_new(sizeof(CallbackData), &callback_data_object_interface);
  CallbackData *self = (CallbackData *)object;
  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callback = callback;
  return object;
}

struct _UtX11Core {
  UtObject object;
  UtObject *client;
  UtObject *callback_object;
  const UtX11EventCallbacks *event_callbacks;
};

static void decode_expose(UtX11Core *self, uint8_t data0, UtObject *data) {
  size_t offset = 0;
  uint32_t window = ut_x11_buffer_get_card32(data, &offset);
  uint16_t x = ut_x11_buffer_get_card16(data, &offset);
  uint16_t y = ut_x11_buffer_get_card16(data, &offset);
  uint16_t width = ut_x11_buffer_get_card16(data, &offset);
  uint16_t height = ut_x11_buffer_get_card16(data, &offset);
  uint16_t count = ut_x11_buffer_get_card16(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 14);

  if (self->callback_object != NULL && self->event_callbacks->expose != NULL) {
    self->event_callbacks->expose(self->callback_object, window, x, y, width,
                                  height, count);
  }
}

static void decode_no_expose(UtX11Core *self, uint8_t data0, UtObject *data) {
  size_t offset = 0;
  uint32_t drawable = ut_x11_buffer_get_card32(data, &offset);
  ut_x11_buffer_get_card16(data, &offset); // minor-opcode
  ut_x11_buffer_get_card8(data, &offset);  // major-opcode

  if (self->callback_object != NULL &&
      self->event_callbacks->no_expose != NULL) {
    self->event_callbacks->no_expose(self->callback_object, drawable);
  }
}

static void decode_map_notify(UtX11Core *self, uint8_t data0, UtObject *data) {
  size_t offset = 0;
  uint32_t event = ut_x11_buffer_get_card32(data, &offset);
  uint32_t window = ut_x11_buffer_get_card32(data, &offset);
  bool override_redirect = ut_x11_buffer_get_bool(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 19);

  if (self->callback_object != NULL &&
      self->event_callbacks->map_notify != NULL) {
    self->event_callbacks->map_notify(self->callback_object, event, window,
                                      override_redirect);
  }
}

static void decode_reparent_notify(UtX11Core *self, uint8_t data0,
                                   UtObject *data) {
  size_t offset = 0;
  uint32_t event = ut_x11_buffer_get_card32(data, &offset);
  uint32_t window = ut_x11_buffer_get_card32(data, &offset);
  uint32_t parent = ut_x11_buffer_get_card32(data, &offset);
  int16_t x = ut_x11_buffer_get_int16(data, &offset);
  int16_t y = ut_x11_buffer_get_int16(data, &offset);
  bool override_redirect = ut_x11_buffer_get_bool(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 11);

  if (self->callback_object != NULL &&
      self->event_callbacks->reparent_notify != NULL) {
    self->event_callbacks->reparent_notify(self->callback_object, event, window,
                                           parent, x, y, override_redirect);
  }
}

static void decode_configure_notify(UtX11Core *self, uint8_t data0,
                                    UtObject *data) {
  size_t offset = 0;
  ut_x11_buffer_get_card32(data, &offset); // event
  uint32_t window = ut_x11_buffer_get_card32(data, &offset);
  ut_x11_buffer_get_card32(data, &offset); // above_sibling
  int16_t x = ut_x11_buffer_get_int16(data, &offset);
  int16_t y = ut_x11_buffer_get_int16(data, &offset);
  uint16_t width = ut_x11_buffer_get_card16(data, &offset);
  uint16_t height = ut_x11_buffer_get_card16(data, &offset);
  ut_x11_buffer_get_card16(data, &offset); // border_width
  ut_x11_buffer_get_card8(data, &offset);  // override_redirect
  ut_x11_buffer_get_padding(data, &offset, 5);

  if (self->callback_object != NULL &&
      self->event_callbacks->configure_notify != NULL) {
    self->event_callbacks->configure_notify(self->callback_object, window, x, y,
                                            width, height);
  }
}

static void decode_property_notify(UtX11Core *self, uint8_t data0,
                                   UtObject *data) {
  size_t offset = 0;
  uint32_t window = ut_x11_buffer_get_card32(data, &offset);
  uint32_t atom = ut_x11_buffer_get_card32(data, &offset);
  uint32_t timestamp = ut_x11_buffer_get_card32(data, &offset);
  uint8_t state = ut_x11_buffer_get_card8(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 15);

  if (self->callback_object != NULL &&
      self->event_callbacks->property_notify != NULL) {
    self->event_callbacks->property_notify(self->callback_object, window, atom,
                                           timestamp, state);
  }
}

static void decode_client_message(UtX11Core *self, uint8_t data0,
                                  UtObject *data) {
  uint8_t format = data0;
  size_t offset = 0;
  uint32_t window = ut_x11_buffer_get_card32(data, &offset);
  uint32_t type = ut_x11_buffer_get_card32(data, &offset);
  UtObjectRef message_data = NULL;
  switch (format) {
  case 8:
    message_data = ut_uint8_list_new();
    for (size_t i = 0; i < 20; i++) {
      ut_uint8_list_append(message_data,
                           ut_x11_buffer_get_card8(data, &offset));
    }
    break;
  case 16:
    message_data = ut_uint16_list_new();
    for (size_t i = 0; i < 10; i++) {
      ut_uint16_list_append(message_data,
                            ut_x11_buffer_get_card16(data, &offset));
    }
    break;
  case 32:
    message_data = ut_uint32_list_new();
    for (size_t i = 0; i < 5; i++) {
      ut_uint32_list_append(message_data,
                            ut_x11_buffer_get_card32(data, &offset));
    }
    break;
  default:
    assert(false);
  }

  if (self->callback_object != NULL &&
      self->event_callbacks->client_message != NULL) {
    self->event_callbacks->client_message(self->callback_object, window, type,
                                          message_data);
  }
}

static void decode_get_window_attributes_reply(UtObject *object, uint8_t data0,
                                               UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  uint8_t backing_store = data0;
  uint32_t visual = ut_x11_buffer_get_card32(data, &offset);
  uint16_t class = ut_x11_buffer_get_card16(data, &offset);
  uint8_t bit_gravity = ut_x11_buffer_get_card8(data, &offset);
  uint8_t win_gravity = ut_x11_buffer_get_card8(data, &offset);
  uint32_t backing_planes = ut_x11_buffer_get_card32(data, &offset);
  uint32_t backing_pixel = ut_x11_buffer_get_card32(data, &offset);
  bool save_under = ut_x11_buffer_get_bool(data, &offset);
  bool map_is_installed = ut_x11_buffer_get_bool(data, &offset);
  uint8_t map_state = ut_x11_buffer_get_card8(data, &offset);
  bool override_redirect = ut_x11_buffer_get_bool(data, &offset);
  uint32_t colormap = ut_x11_buffer_get_card32(data, &offset);
  uint32_t all_event_masks = ut_x11_buffer_get_card32(data, &offset);
  uint32_t your_event_mask = ut_x11_buffer_get_card32(data, &offset);
  uint16_t do_not_propagate_mask = ut_x11_buffer_get_card16(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 2);
  ut_x11_buffer_get_padding(data, &offset, 20);

  if (callback_data->callback_object != NULL &&
      callback_data->callback != NULL) {
    UtX11GetWindowAttributesCallback callback =
        (UtX11GetWindowAttributesCallback)callback_data->callback;
    callback(callback_data->callback_object, visual, class, bit_gravity,
             win_gravity, backing_store, backing_planes, backing_pixel,
             save_under, map_is_installed, map_state, override_redirect,
             colormap, all_event_masks, your_event_mask, do_not_propagate_mask,
             NULL);
  }
}

static void handle_get_window_attributes_error(UtObject *object,
                                               UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback_object != NULL &&
      callback_data->callback != NULL) {
    UtX11GetWindowAttributesCallback callback =
        (UtX11GetWindowAttributesCallback)callback_data->callback;
    callback(callback_data->callback_object, 0, 0, 0, 0, 0, 0, 0, false, false,
             0, false, 0, 0, 0, 0, error);
  }
}

static void decode_intern_atom_reply(UtObject *object, uint8_t data0,
                                     UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  uint32_t atom = ut_x11_buffer_get_card32(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 20);

  if (callback_data->callback_object != NULL &&
      callback_data->callback != NULL) {
    UtX11InternAtomCallback callback =
        (UtX11InternAtomCallback)callback_data->callback;
    callback(callback_data->callback_object, atom, NULL);
  }
}

static void handle_intern_atom_error(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback_object != NULL &&
      callback_data->callback != NULL) {
    UtX11InternAtomCallback callback =
        (UtX11InternAtomCallback)callback_data->callback;
    callback(callback_data->callback_object, 0, error);
  }
}

static void decode_get_atom_name_reply(UtObject *object, uint8_t data0,
                                       UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  uint16_t name_length = ut_x11_buffer_get_card16(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 22);
  ut_cstring_ref name = ut_x11_buffer_get_string8(data, &offset, name_length);
  ut_x11_buffer_get_align_padding(data, &offset, 4);

  if (callback_data->callback_object != NULL &&
      callback_data->callback != NULL) {
    UtX11GetAtomNameCallback callback =
        (UtX11GetAtomNameCallback)callback_data->callback;
    callback(callback_data->callback_object, name, NULL);
  }
}

static void handle_get_atom_name_error(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback_object != NULL &&
      callback_data->callback != NULL) {
    UtX11GetAtomNameCallback callback =
        (UtX11GetAtomNameCallback)callback_data->callback;
    callback(callback_data->callback_object, NULL, error);
  }
}

static void decode_get_property_reply(UtObject *object, uint8_t data0,
                                      UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  uint8_t format = data0;
  uint32_t type = ut_x11_buffer_get_card32(data, &offset);
  uint32_t bytes_after = ut_x11_buffer_get_card32(data, &offset);
  size_t length = ut_x11_buffer_get_card32(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 12);
  UtObjectRef value = NULL;
  if (format == 0) {
  } else if (format == 8) {
    value = ut_uint8_array_new();
    for (size_t i = 0; i < length; i++) {
      ut_uint8_list_append(value, ut_x11_buffer_get_card8(data, &offset));
    }
  } else if (format == 16) {
    value = ut_uint16_list_new();
    for (size_t i = 0; i < length; i++) {
      ut_uint16_list_append(value, ut_x11_buffer_get_card16(data, &offset));
    }
  } else if (format == 32) {
    value = ut_uint32_list_new();
    for (size_t i = 0; i < length; i++) {
      ut_uint32_list_append(value, ut_x11_buffer_get_card32(data, &offset));
    }
  } else {
    assert(false);
  }
  ut_x11_buffer_get_align_padding(data, &offset, 4);

  if (callback_data->callback_object != NULL &&
      callback_data->callback != NULL) {
    UtX11GetPropertyCallback callback =
        (UtX11GetPropertyCallback)callback_data->callback;
    callback(callback_data->callback_object, type, value, bytes_after, NULL);
  }
}

static void handle_get_property_error(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback_object != NULL &&
      callback_data->callback != NULL) {
    UtX11GetPropertyCallback callback =
        (UtX11GetPropertyCallback)callback_data->callback;
    callback(callback_data->callback_object, 0, NULL, 0, error);
  }
}

static void decode_list_properties_reply(UtObject *object, uint8_t data0,
                                         UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  size_t atoms_length = ut_x11_buffer_get_card16(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 22);
  UtObjectRef atoms = ut_uint32_list_new();
  for (size_t i = 0; i < atoms_length; i++) {
    ut_uint32_list_append(atoms, ut_x11_buffer_get_card32(data, &offset));
  }

  if (callback_data->callback_object != NULL &&
      callback_data->callback != NULL) {
    UtX11ListPropertiesCallback callback =
        (UtX11ListPropertiesCallback)callback_data->callback;
    callback(callback_data->callback_object, atoms, NULL);
  }
}

static void handle_list_properties_error(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback_object != NULL &&
      callback_data->callback != NULL) {
    UtX11ListPropertiesCallback callback =
        (UtX11ListPropertiesCallback)callback_data->callback;
    callback(callback_data->callback_object, NULL, error);
  }
}

static void decode_get_image_reply(UtObject *object, uint8_t data0,
                                   UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  uint8_t depth = data0;
  size_t offset = 0;
  uint32_t visual = ut_x11_buffer_get_card32(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 20);
  size_t data_length = ut_list_get_length(data);
  UtObjectRef image_data = ut_list_get_sublist(data, offset, data_length);

  if (callback_data->callback_object != NULL &&
      callback_data->callback != NULL) {
    UtX11GetImageCallback callback =
        (UtX11GetImageCallback)callback_data->callback;
    callback(callback_data->callback_object, visual, depth, image_data, NULL);
  }
}

static void handle_get_image_error(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback_object != NULL &&
      callback_data->callback != NULL) {
    UtX11GetImageCallback callback =
        (UtX11GetImageCallback)callback_data->callback;
    callback(callback_data->callback_object, 0, 0, NULL, error);
  }
}

static void decode_query_extension_reply(UtObject *object, uint8_t data0,
                                         UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  bool present = ut_x11_buffer_get_bool(data, &offset);
  uint8_t major_opcode = ut_x11_buffer_get_card8(data, &offset);
  uint8_t first_event = ut_x11_buffer_get_card8(data, &offset);
  uint8_t first_error = ut_x11_buffer_get_card8(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 20);

  if (callback_data->callback_object != NULL &&
      callback_data->callback != NULL) {
    UtX11QueryExtensionCallback callback =
        (UtX11QueryExtensionCallback)callback_data->callback;
    callback(callback_data->callback_object, present, major_opcode, first_event,
             first_error, NULL);
  }
}

static void handle_query_extension_error(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback_object != NULL &&
      callback_data->callback != NULL) {
    UtX11QueryExtensionCallback callback =
        (UtX11QueryExtensionCallback)callback_data->callback;
    callback(callback_data->callback_object, false, 0, 0, 0, error);
  }
}

static void decode_list_extensions_reply(UtObject *object, uint8_t data0,
                                         UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  ut_x11_buffer_get_padding(data, &offset, 24);
  size_t names_length = data0;
  UtObjectRef names = ut_string_list_new();
  for (size_t i = 0; i < names_length; i++) {
    uint8_t name_length = ut_x11_buffer_get_card8(data, &offset);
    ut_cstring_ref name = ut_x11_buffer_get_string8(data, &offset, name_length);
    ut_string_list_append(names, name);
  }
  ut_x11_buffer_get_align_padding(data, &offset, 4);

  if (callback_data->callback_object != NULL &&
      callback_data->callback != NULL) {
    UtX11ListExtensionsCallback callback =
        (UtX11ListExtensionsCallback)callback_data->callback;
    callback(callback_data->callback_object, names, NULL);
  }
}

static void handle_list_extensions_error(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback_object != NULL &&
      callback_data->callback != NULL) {
    UtX11ListExtensionsCallback callback =
        (UtX11ListExtensionsCallback)callback_data->callback;
    callback(callback_data->callback_object, NULL, error);
  }
}

static void ut_x11_core_cleanup(UtObject *object) {
  UtX11Core *self = (UtX11Core *)object;
  ut_object_weak_unref(&self->callback_object);
}

static uint8_t ut_x11_core_get_major_opcode(UtObject *object) { return 0; }

static uint8_t ut_x11_core_get_first_event(UtObject *object) { return 0; }

static bool ut_x11_core_decode_event(UtObject *object, uint8_t code,
                                     bool from_send_event,
                                     uint16_t sequence_number, uint8_t data0,
                                     UtObject *data) {
  UtX11Core *self = (UtX11Core *)object;

  switch (code) {
  case 12:
    decode_expose(self, data0, data);
    return true;
  case 14:
    decode_no_expose(self, data0, data);
    return true;
  case 19:
    decode_map_notify(self, data0, data);
    return true;
  case 21:
    decode_reparent_notify(self, data0, data);
    return true;
  case 22:
    decode_configure_notify(self, data0, data);
    return true;
  case 28:
    decode_property_notify(self, data0, data);
    return true;
  case 33:
    decode_client_message(self, data0, data);
    return true;
  default:
    return false;
  }
}

static UtX11ErrorCode ut_x11_core_decode_error(UtObject *object, uint8_t code) {
  switch (code) {
  case 1:
    return UT_X11_ERROR_REQUEST;
  case 2:
    return UT_X11_ERROR_VALUE;
  case 3:
    return UT_X11_ERROR_WINDOW;
  case 4:
    return UT_X11_ERROR_PIXMAP;
  case 5:
    return UT_X11_ERROR_ATOM;
  case 6:
    return UT_X11_ERROR_CURSOR;
  case 7:
    return UT_X11_ERROR_FONT;
  case 8:
    return UT_X11_ERROR_MATCH;
  case 9:
    return UT_X11_ERROR_DRAWABLE;
  case 10:
    return UT_X11_ERROR_ACCESS;
  case 11:
    return UT_X11_ERROR_ALLOC;
  case 12:
    return UT_X11_ERROR_COLORMAP;
  case 13:
    return UT_X11_ERROR_GCONTEXT;
  case 14:
    return UT_X11_ERROR_ID_CHOICE;
  case 15:
    return UT_X11_ERROR_NAME;
  case 16:
    return UT_X11_ERROR_LENGTH;
  case 17:
    return UT_X11_ERROR_IMPLEMENTATION;
  default:
    return UT_X11_ERROR_UNKNOWN;
  }
}

static void ut_x11_core_close(UtObject *object) {
  UtX11Core *self = (UtX11Core *)object;
  self->client = NULL;
}

static UtX11ExtensionInterface x11_extension_interface = {
    .get_major_opcode = ut_x11_core_get_major_opcode,
    .get_first_event = ut_x11_core_get_first_event,
    .decode_event = ut_x11_core_decode_event,
    .decode_error = ut_x11_core_decode_error,
    .close = ut_x11_core_close};

static UtObjectInterface object_interface = {
    .type_name = "UtX11Core",
    .cleanup = ut_x11_core_cleanup,
    .interfaces = {{&ut_x11_extension_id, &x11_extension_interface},
                   {NULL, NULL}}};

UtObject *ut_x11_core_new(UtObject *client, UtObject *callback_object,
                          const UtX11EventCallbacks *event_callbacks) {
  UtObject *object = ut_object_new(sizeof(UtX11Core), &object_interface);
  UtX11Core *self = (UtX11Core *)object;
  self->client = client;
  ut_object_weak_ref(callback_object, &self->callback_object);
  self->event_callbacks = event_callbacks;
  return object;
}

uint32_t ut_x11_core_create_window(UtObject *object, uint32_t parent, int16_t x,
                                   int16_t y, uint16_t width, uint16_t height,
                                   uint8_t depth, uint32_t visual,
                                   uint32_t event_mask) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  uint32_t id = ut_x11_client_create_resource_id(self->client);

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, id);
  ut_x11_buffer_append_card32(request, parent);
  ut_x11_buffer_append_int16(request, x);
  ut_x11_buffer_append_int16(request, y);
  ut_x11_buffer_append_card16(request, width);
  ut_x11_buffer_append_card16(request, height);
  ut_x11_buffer_append_card16(request, 0); // border_width
  ut_x11_buffer_append_card16(request, WINDOW_CLASS_INPUT_OUTPUT);
  ut_x11_buffer_append_card32(request, visual);
  ut_x11_buffer_append_card32(request, VALUE_MASK_EVENT_MASK);
  ut_x11_buffer_append_card32(request, event_mask);

  ut_x11_client_send_request(self->client, 1, depth, request);

  return id;
}

void ut_x11_core_change_window_attributes(UtObject *object, uint32_t window,
                                          uint32_t event_mask) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);
  ut_x11_buffer_append_card32(request, VALUE_MASK_EVENT_MASK);
  ut_x11_buffer_append_card32(request, event_mask);

  ut_x11_client_send_request(self->client, 2, 0, request);
}

void ut_x11_core_get_window_attributes(
    UtObject *object, uint32_t window, UtObject *callback_object,
    UtX11GetWindowAttributesCallback callback) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);

  ut_x11_client_send_request_with_reply(
      self->client, 3, 0, request, callback_data_new(callback_object, callback),
      decode_get_window_attributes_reply, handle_get_window_attributes_error);
}

void ut_x11_core_destroy_window(UtObject *object, uint32_t window) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);

  ut_x11_client_send_request(self->client, 4, 0, request);
}

void ut_x11_core_destroy_subwindows(UtObject *object, uint32_t window) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);

  ut_x11_client_send_request(self->client, 5, 0, request);
}

// ChangeSaveSet

void ut_x11_core_reparent_window(UtObject *object, uint32_t window,
                                 uint32_t parent, int16_t x, int16_t y) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);
  ut_x11_buffer_append_card32(request, parent);
  ut_x11_buffer_append_int16(request, x);
  ut_x11_buffer_append_int16(request, y);

  ut_x11_client_send_request(self->client, 7, 0, request);
}

void ut_x11_core_map_window(UtObject *object, uint32_t window) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);

  ut_x11_client_send_request(self->client, 8, 0, request);
}

void ut_x11_core_map_subwindows(UtObject *object, uint32_t window) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);

  ut_x11_client_send_request(self->client, 9, 0, request);
}

void ut_x11_core_unmap_window(UtObject *object, uint32_t window) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);

  ut_x11_client_send_request(self->client, 10, 0, request);
}

void ut_x11_core_unmap_subwindows(UtObject *object, uint32_t window) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);

  ut_x11_client_send_request(self->client, 11, 0, request);
}

void ut_x11_core_configure_window(UtObject *object, uint32_t window, int16_t x,
                                  int16_t y, uint16_t width, uint16_t height) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);
  ut_x11_buffer_append_card16(request, 0x0001 | 0x0002 | 0x0004 | 0x0008);
  ut_x11_buffer_append_padding(request, 2);
  ut_x11_buffer_append_value_int16(request, x);
  ut_x11_buffer_append_value_int16(request, y);
  ut_x11_buffer_append_value_card16(request, width);
  ut_x11_buffer_append_value_card16(request, height);

  ut_x11_client_send_request(self->client, 12, 0, request);
}

// CirculateWindow

// GetGeometry

// QueryTree

void ut_x11_core_intern_atom(UtObject *object, const char *name,
                             bool only_if_exists, UtObject *callback_object,
                             UtX11InternAtomCallback callback) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card16(request, ut_cstring_get_length(name));
  ut_x11_buffer_append_padding(request, 2);
  ut_x11_buffer_append_string8(request, name);
  ut_x11_buffer_append_align_padding(request, 4);

  ut_x11_client_send_request_with_reply(
      self->client, 16, only_if_exists ? 1 : 0, request,
      callback_data_new(callback_object, callback), decode_intern_atom_reply,
      handle_intern_atom_error);
}

void ut_x11_core_get_atom_name(UtObject *object, uint32_t atom,
                               UtObject *callback_object,
                               UtX11GetAtomNameCallback callback) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, atom);

  ut_x11_client_send_request_with_reply(
      self->client, 17, 0, request,
      callback_data_new(callback_object, callback), decode_get_atom_name_reply,
      handle_get_atom_name_error);
}

void ut_x11_core_change_property_uint8(UtObject *object, uint32_t window,
                                       uint32_t property,
                                       UtX11PropertyMode mode, uint32_t type,
                                       UtObject *data) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);
  ut_x11_buffer_append_card32(request, property);
  ut_x11_buffer_append_card32(request, type);
  ut_x11_buffer_append_card8(request, 8); // format
  ut_x11_buffer_append_padding(request, 3);
  size_t data_length = ut_list_get_length(data);
  ut_x11_buffer_append_card32(request, data_length);
  for (size_t i = 0; i < data_length; i++) {
    ut_x11_buffer_append_card8(request, ut_uint8_list_get_element(data, i));
  }
  ut_x11_buffer_append_align_padding(request, 4);

  ut_x11_client_send_request(self->client, 18, mode, request);
}

void ut_x11_core_change_property_uint16(UtObject *object, uint32_t window,
                                        uint32_t property,
                                        UtX11PropertyMode mode, uint32_t type,
                                        UtObject *data) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);
  ut_x11_buffer_append_card32(request, property);
  ut_x11_buffer_append_card32(request, type);
  ut_x11_buffer_append_card8(request, 16); // format
  ut_x11_buffer_append_padding(request, 3);
  size_t data_length = ut_list_get_length(data);
  ut_x11_buffer_append_card32(request, data_length);
  for (size_t i = 0; i < data_length; i++) {
    ut_x11_buffer_append_card16(request, ut_uint16_list_get_element(data, i));
  }
  ut_x11_buffer_append_align_padding(request, 4);

  ut_x11_client_send_request(self->client, 18, mode, request);
}

void ut_x11_core_change_property_uint32(UtObject *object, uint32_t window,
                                        uint32_t property,
                                        UtX11PropertyMode mode, uint32_t type,
                                        UtObject *data) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);
  ut_x11_buffer_append_card32(request, property);
  ut_x11_buffer_append_card32(request, type);
  ut_x11_buffer_append_card8(request, 32); // format
  ut_x11_buffer_append_padding(request, 3);
  size_t data_length = ut_list_get_length(data);
  ut_x11_buffer_append_card32(request, data_length);
  for (size_t i = 0; i < data_length; i++) {
    ut_x11_buffer_append_card32(request, ut_uint32_list_get_element(data, i));
  }

  ut_x11_client_send_request(self->client, 18, mode, request);
}

void ut_x11_core_change_property_string(UtObject *object, uint32_t window,
                                        uint32_t property,
                                        UtX11PropertyMode mode,
                                        const char *value) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);
  ut_x11_buffer_append_card32(request, property);
  ut_x11_buffer_append_card32(request, UT_X11_STRING);
  ut_x11_buffer_append_card8(request, 8); // format
  ut_x11_buffer_append_padding(request, 3);
  ut_x11_buffer_append_card32(request, ut_cstring_get_length(value));
  ut_x11_buffer_append_string8(request, value);
  ut_x11_buffer_append_align_padding(request, 4);

  ut_x11_client_send_request(self->client, 18, mode, request);
}

void ut_x11_core_delete_property(UtObject *object, uint32_t window,
                                 uint32_t property) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);
  ut_x11_buffer_append_card32(request, property);

  ut_x11_client_send_request(self->client, 19, 0, request);
}

void ut_x11_core_get_property(UtObject *object, uint32_t window,
                              uint32_t property, uint32_t type,
                              uint32_t long_offset, uint32_t long_length,
                              bool delete, UtObject *callback_object,
                              UtX11GetPropertyCallback callback) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);
  ut_x11_buffer_append_card32(request, property);
  ut_x11_buffer_append_card32(request, type);
  ut_x11_buffer_append_card32(request, long_offset);
  ut_x11_buffer_append_card32(request, long_length);

  ut_x11_client_send_request_with_reply(
      self->client, 20, delete ? 1 : 0, request,
      callback_data_new(callback_object, callback), decode_get_property_reply,
      handle_get_property_error);
}

void ut_x11_core_list_properties(UtObject *object, uint32_t window,
                                 UtObject *callback_object,
                                 UtX11ListPropertiesCallback callback) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);

  ut_x11_client_send_request_with_reply(
      self->client, 21, 0, request,
      callback_data_new(callback_object, callback),
      decode_list_properties_reply, handle_list_properties_error);
}

void ut_x11_core_grab_server(UtObject *object) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  ut_x11_client_send_request(self->client, 36, 0, NULL);
}

void ut_x11_core_ungrab_server(UtObject *object) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  ut_x11_client_send_request(self->client, 37, 0, NULL);
}

uint32_t ut_x11_core_create_pixmap(UtObject *object, uint32_t drawable,
                                   uint16_t width, uint16_t height,
                                   uint8_t depth) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  uint32_t id = ut_x11_client_create_resource_id(self->client);

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, id);
  ut_x11_buffer_append_card32(request, drawable);
  ut_x11_buffer_append_card16(request, width);
  ut_x11_buffer_append_card16(request, height);

  ut_x11_client_send_request(self->client, 53, depth, request);

  return id;
}

void ut_x11_core_free_pixmap(UtObject *object, uint32_t pixmap) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, pixmap);

  ut_x11_client_send_request(self->client, 54, 0, request);
}

uint32_t ut_x11_core_create_gc(UtObject *object, uint32_t drawable) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  uint32_t id = ut_x11_client_create_resource_id(self->client);

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, id);
  ut_x11_buffer_append_card32(request, drawable);
  uint32_t mask = 0x00000000;
  ut_x11_buffer_append_card32(request, mask);
  // FIXME: values

  ut_x11_client_send_request(self->client, 55, 0, request);

  return id;
}

void ut_x11_core_change_gc(UtObject *object, uint32_t gc) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, gc);
  ut_x11_buffer_append_card32(request, 0);

  ut_x11_client_send_request(self->client, 56, 0, request);
}

void ut_x11_core_copy_gc(UtObject *object, uint32_t src_gc, uint32_t dst_gc,
                         uint32_t value_mask) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, src_gc);
  ut_x11_buffer_append_card32(request, dst_gc);
  ut_x11_buffer_append_card32(request, value_mask);

  ut_x11_client_send_request(self->client, 57, 0, request);
}

void ut_x11_core_free_gc(UtObject *object, uint32_t gc) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, gc);

  ut_x11_client_send_request(self->client, 60, 0, request);
}

void ut_x11_core_clear_area(UtObject *object, uint32_t window, int16_t x,
                            int16_t y, uint16_t width, uint16_t height,
                            bool exposures) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);
  ut_x11_buffer_append_int16(request, x);
  ut_x11_buffer_append_int16(request, y);
  ut_x11_buffer_append_card16(request, width);
  ut_x11_buffer_append_card16(request, height);

  ut_x11_client_send_request(self->client, 61, exposures ? 1 : 0, request);
}

void ut_x11_core_copy_area(UtObject *object, uint32_t src_drawable,
                           uint32_t dst_drawable, uint32_t gc, int16_t src_x,
                           int16_t src_y, int16_t dst_x, int16_t dst_y,
                           uint16_t width, uint16_t height) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, src_drawable);
  ut_x11_buffer_append_card32(request, dst_drawable);
  ut_x11_buffer_append_card32(request, gc);
  ut_x11_buffer_append_int16(request, src_x);
  ut_x11_buffer_append_int16(request, src_y);
  ut_x11_buffer_append_int16(request, dst_x);
  ut_x11_buffer_append_int16(request, dst_y);
  ut_x11_buffer_append_card16(request, width);
  ut_x11_buffer_append_card16(request, height);

  ut_x11_client_send_request(self->client, 62, 0, request);
}

void ut_x11_core_put_image(UtObject *object, uint32_t drawable, uint32_t gc,
                           UtX11ImageFormat format, uint16_t width,
                           uint16_t height, uint8_t depth, int16_t dst_x,
                           int16_t dst_y, UtObject *data) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, drawable);
  ut_x11_buffer_append_card32(request, gc);
  ut_x11_buffer_append_card16(request, width);
  ut_x11_buffer_append_card16(request, height);
  ut_x11_buffer_append_int16(request, dst_x);
  ut_x11_buffer_append_int16(request, dst_y);
  ut_x11_buffer_append_card8(request, 0); // left_pad);
  ut_x11_buffer_append_card8(request, depth);
  ut_x11_buffer_append_padding(request, 2);
  ut_x11_buffer_append_list(request, data);
  ut_x11_buffer_append_align_padding(request, 4);

  ut_x11_client_send_request(self->client, 72, format, request);
}

void ut_x11_core_get_image(UtObject *object, uint32_t drawable,
                           UtX11ImageFormat format, int16_t x, int16_t y,
                           uint16_t width, uint16_t height, uint32_t plane_mask,
                           UtObject *callback_object,
                           UtX11GetImageCallback callback) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, drawable);
  ut_x11_buffer_append_int16(request, x);
  ut_x11_buffer_append_int16(request, y);
  ut_x11_buffer_append_card16(request, width);
  ut_x11_buffer_append_card16(request, height);
  ut_x11_buffer_append_card32(request, plane_mask);

  ut_x11_client_send_request_with_reply(
      self->client, 73, format, request,
      callback_data_new(callback_object, callback), decode_get_image_reply,
      handle_get_image_error);
}

void ut_x11_core_query_extension(UtObject *object, const char *name,
                                 UtObject *callback_object,
                                 UtX11QueryExtensionCallback callback) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card16(request, ut_cstring_get_length(name));
  ut_x11_buffer_append_padding(request, 2);
  ut_x11_buffer_append_string8(request, name);
  ut_x11_buffer_append_align_padding(request, 4);

  ut_x11_client_send_request_with_reply(
      self->client, 98, 0, request,
      callback_data_new(callback_object, callback),
      decode_query_extension_reply, handle_query_extension_error);
}

void ut_x11_core_list_extensions(UtObject *object, UtObject *callback_object,
                                 UtX11ListExtensionsCallback callback) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  ut_x11_client_send_request_with_reply(
      self->client, 99, 0, NULL, callback_data_new(callback_object, callback),
      decode_list_extensions_reply, handle_list_extensions_error);
}

void ut_x11_core_bell(UtObject *object) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  ut_x11_client_send_request(self->client, 108, 0, NULL);
}

void ut_x11_core_kill_client(UtObject *object, uint32_t resource) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, resource);

  ut_x11_client_send_request(self->client, 113, 0, request);
}

void ut_x11_core_rotate_properties(UtObject *object, uint32_t window,
                                   int16_t delta, UtObject *atoms) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  size_t atoms_length = ut_list_get_length(atoms);

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);
  ut_x11_buffer_append_card16(request, atoms_length);
  ut_x11_buffer_append_int16(request, delta);
  for (size_t i = 0; i < atoms_length; i++) {
    ut_x11_buffer_append_card32(request, ut_uint32_list_get_element(atoms, i));
  }

  ut_x11_client_send_request(self->client, 114, 0, request);
}

void ut_x11_core_no_operation(UtObject *object) {
  assert(ut_object_is_x11_core(object));
  UtX11Core *self = (UtX11Core *)object;

  ut_x11_client_send_request(self->client, 127, 0, NULL);
}

bool ut_object_is_x11_core(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
