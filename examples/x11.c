#include <assert.h>
#include <stdio.h>

#include "ut.h"

static size_t pixmap_width = 0;
static size_t pixmap_height = 0;

static UtObject *client = NULL;
static UtObject *atoms = NULL;
static uint32_t segment = 0;
static UtObject *buffer = NULL;
static uint32_t window = 0;
static uint32_t pixmap = 0;
static uint32_t gc = 0;

static uint32_t get_atom(const char *name) {
  UtObject *value = ut_map_lookup_string(atoms, name);
  if (value == NULL) {
    return 0;
  }

  return ut_uint32_get_value(value);
}

static void enter_cb(UtObject *object, uint16_t device_id, uint32_t timestamp,
                     UtX11InputNotifyMode mode, UtX11InputNotifyDetail detail,
                     uint32_t window, double x, double y) {
  printf("Enter\n");
}

static void leave_cb(UtObject *object, uint16_t device_id, uint32_t timestamp,
                     UtX11InputNotifyMode mode, UtX11InputNotifyDetail detail,
                     uint32_t window, double x, double y) {
  printf("Leave\n");
}

static void motion_cb(UtObject *object, uint16_t device_id, uint32_t timestamp,
                      uint32_t window, double x, double y,
                      UtX11PointerEventFlag flag) {
  printf("Motion (%f,%f)\n", x, y);
}

static void button_press_cb(UtObject *object, uint16_t device_id,
                            uint32_t timestamp, uint32_t window, uint8_t button,
                            double x, double y, UtX11PointerEventFlag flag) {
  printf("ButtonPress %d\n", button);
}

static void button_release_cb(UtObject *object, uint16_t device_id,
                              uint32_t timestamp, uint32_t window,
                              uint8_t button, double x, double y,
                              UtX11PointerEventFlag flag) {
  printf("ButtonRelease %d\n", button);
}

static void focus_in_cb(UtObject *object, uint16_t device_id,
                        uint32_t timestamp, UtX11InputNotifyMode mode,
                        UtX11InputNotifyDetail detail, uint32_t window) {
  printf("FocusIn\n");
}

static void focus_out_cb(UtObject *object, uint16_t device_id,
                         uint32_t timestamp, UtX11InputNotifyMode mode,
                         UtX11InputNotifyDetail detail, uint32_t window) {
  printf("FocusOut\n");
}

static void key_press_cb(UtObject *object, uint16_t device_id,
                         uint32_t timestamp, uint32_t window, uint8_t keycode,
                         double x, double y, UtX11KeyEventFlag flag) {
  printf("KeyPress %d\n", keycode);
}

static void key_release_cb(UtObject *object, uint16_t device_id,
                           uint32_t timestamp, uint32_t window, uint8_t keycode,
                           double x, double y, UtX11KeyEventFlag flag) {
  printf("KeyRelease %d\n", keycode);
}

static void client_message_cb(UtObject *object, uint32_t window, uint32_t type,
                              UtObject *data) {
  printf("ClientMessage\n");

  if (type == get_atom("WM_PROTOCOLS")) {
    if (ut_uint32_list_get_element(data, 0) == get_atom("WM_DELETE_WINDOW")) {
      ut_event_loop_return(NULL);
    }
  }
}

static void configure_notify_cb(UtObject *object, uint32_t window, int16_t x,
                                int16_t y, uint16_t width, uint16_t height) {
  printf("ConfigureNotify (%d,%d) %dx%d\n", x, y, width, height);
  if (width == pixmap_width && height == pixmap_height) {
    return;
  }

  pixmap_width = width;
  pixmap_height = height;

  ut_object_unref(buffer);
  buffer = ut_shared_memory_array_new(width * height * 4);
  uint8_t *pixmap_data = ut_shared_memory_array_get_data(buffer);
  for (size_t y = 0; y < height; y++) {
    for (size_t x = 0; x < width; x++) {
      uint8_t *pixel = pixmap_data + (y * width * 4) + (x * 4);
      pixel[0] = 255 * x / width;
      pixel[1] = 255 * y / height;
      pixel[2] = 255;
      pixel[3] = 255;
    }
  }

  if (segment != 0) {
    ut_x11_client_shm_detach(client, segment);
  }
  segment = ut_x11_client_shm_attach_fd(
      client, ut_shared_memory_array_get_fd(buffer), false);
  if (pixmap != 0) {
    ut_x11_client_free_pixmap(client, pixmap);
  }
  pixmap = ut_x11_client_shm_create_pixmap(client, window, width, height, 24,
                                           segment, 0);
  gc = ut_x11_client_create_gc(client, pixmap);
}

static void expose_cb(UtObject *object, uint32_t window, uint16_t x, uint16_t y,
                      uint16_t width, uint16_t height, uint16_t count) {
  printf("Expose (%d,%d) %dx%d\n", x, y, width, height);
  ut_x11_client_copy_area(client, pixmap, window, gc, x, y, x, y, width,
                          height);
}

static void get_wm_state_cb(UtObject *object, uint32_t type, UtObject *value,
                            uint32_t bytes_after, UtObject *error) {
  size_t atoms_length = ut_list_get_length(value);
  bool maximized_vert = false, maximized_horz = false, hidden = false;
  for (size_t i = 0; i < atoms_length; i++) {
    uint32_t atom = ut_uint32_list_get_element(value, i);
    if (atom == get_atom("_NET_WM_STATE_MAXIMIZED_VERT")) {
      maximized_vert = true;
    } else if (atom == get_atom("_NET_WM_STATE_MAXIMIZED_HORZ")) {
      maximized_horz = true;
    } else if (atom == get_atom("_NET_WM_STATE_HIDDEN")) {
      hidden = true;
    }
  }
  printf("Window state: maximized-vert=%s maximized-horz=%s hidden=%s\n",
         maximized_vert ? "yes" : "no", maximized_horz ? "yes" : "no",
         hidden ? "yes" : "no");
}

static void property_notify_cb(UtObject *object, uint32_t window, uint32_t atom,
                               uint32_t timestamp,
                               UtX11PropertyNotifyState state) {
  if (atom == get_atom("_NET_WM_STATE")) {
    ut_x11_client_get_property(client, window, atom, UT_X11_ATOM, object,
                               get_wm_state_cb);
  }
}

static UtX11EventCallbacks event_callbacks = {
    .enter = enter_cb,
    .leave = leave_cb,
    .motion = motion_cb,
    .button_press = button_press_cb,
    .button_release = button_release_cb,
    .focus_in = focus_in_cb,
    .focus_out = focus_out_cb,
    .key_press = key_press_cb,
    .key_release = key_release_cb,
    .client_message = client_message_cb,
    .configure_notify = configure_notify_cb,
    .expose = expose_cb,
    .property_notify = property_notify_cb};

static void error_cb(UtObject *object, UtObject *error) {
  ut_cstring_ref s = ut_object_to_string(error);
  printf("%s\n", s);
}

static void list_extensions_cb(UtObject *object, UtObject *names,
                               UtObject *error) {
  assert(error == NULL);
  size_t names_length = ut_list_get_length(names);
  for (size_t i = 0; i < names_length; i++) {
    printf("%s\n", ut_string_list_get_element(names, i));
  }
}

static void query_device_cb(UtObject *object, UtObject *infos,
                            UtObject *error) {
  size_t infos_length = ut_list_get_length(infos);
  for (size_t i = 0; i < infos_length; i++) {
    UtObjectRef info = ut_list_get_element(infos, i);
    printf("%s\n", ut_x11_input_device_info_get_name(info));
  }
}

static void intern_atom_cb(UtObject *object, uint32_t atom, UtObject *error) {
  UtObjectRef name = object;

  assert(error == NULL);
  ut_map_insert_string_take(atoms, ut_string_get_text(name),
                            ut_uint32_new(atom));

  if (ut_cstring_equal(ut_string_get_text(name), "WM_DELETE_WINDOW")) {
    UtObjectRef protocols =
        ut_uint32_list_new_from_elements(1, get_atom("WM_DELETE_WINDOW"));
    ut_x11_client_set_property_uint32(
        client, window, get_atom("WM_PROTOCOLS"),
        UT_X11_ATOM, protocols);
  }
}

static void intern_atom(const char *name) {
  ut_x11_client_intern_atom(client, name, false, ut_string_new(name),
                            intern_atom_cb);
}

static void connect_cb(UtObject *object, UtObject *error) {
  if (error != NULL) {
    ut_cstring_ref description = ut_error_get_description(error);
    printf("Error connecting: %s", description);
    return;
  }

  printf("Connected\n");

  ut_x11_client_list_extensions(client, object, list_extensions_cb);

  ut_x11_client_query_device(client, UT_X11_DEVICE_ALL_MASTER, object,
                             query_device_cb);

  intern_atom("WM_PROTOCOLS");
  intern_atom("WM_DELETE_WINDOW");
  intern_atom("_NET_WM_STATE");
  intern_atom("_NET_WM_STATE_MODAL");
  intern_atom("_NET_WM_STATE_STICKY");
  intern_atom("_NET_WM_STATE_MAXIMIZED_VERT");
  intern_atom("_NET_WM_STATE_MAXIMIZED_HORZ");
  intern_atom("_NET_WM_STATE_SHADED");
  intern_atom("_NET_WM_STATE_SKIP_TASKBAR");
  intern_atom("_NET_WM_STATE_SKIP_PAGER");
  intern_atom("_NET_WM_STATE_HIDDEN");
  intern_atom("_NET_WM_STATE_FULLSCREEN");
  intern_atom("_NET_WM_STATE_ABOVE");
  intern_atom("_NET_WM_STATE_BELOW");
  intern_atom("_NET_WM_STATE_DEMANDS_ATTENTION");

  window = ut_x11_client_create_window(client, 0, 0, 640, 480,
                                       UT_X11_EVENT_STRUCTURE_NOTIFY |
                                           UT_X11_EVENT_EXPOSURE |
                                           UT_X11_EVENT_PROPERTY_CHANGE);

  UtObjectRef masks = ut_list_new();
  UtObjectRef mask = ut_x11_input_event_mask_new(
      UT_X11_DEVICE_ALL_MASTER,
      UT_X11_INPUT_EVENT_KEY_PRESS | UT_X11_INPUT_EVENT_KEY_RELEASE |
          UT_X11_INPUT_EVENT_BUTTON_PRESS | UT_X11_INPUT_EVENT_BUTTON_RELEASE |
          UT_X11_INPUT_EVENT_MOTION | UT_X11_INPUT_EVENT_ENTER |
          UT_X11_INPUT_EVENT_LEAVE | UT_X11_INPUT_EVENT_FOCUS_IN |
          UT_X11_INPUT_EVENT_FOCUS_OUT | UT_X11_INPUT_EVENT_TOUCH_BEGIN |
          UT_X11_INPUT_EVENT_TOUCH_UPDATE | UT_X11_INPUT_EVENT_TOUCH_END);
  ut_list_append(masks, mask);
  ut_x11_client_select_input_events(client, window, masks);

  ut_x11_client_set_property_string(client, window, UT_X11_WM_NAME,"UT");

  ut_x11_client_map_window(client, window);
}

int main(int argc, char **argv) {
  UtObjectRef dummy_object = ut_null_new();
  client = ut_x11_client_new(dummy_object, &event_callbacks, error_cb);
  atoms = ut_map_new();
  ut_x11_client_connect(client, dummy_object, connect_cb);

  ut_event_loop_run();

  ut_object_unref(client);
  ut_object_unref(atoms);
  ut_object_unref(buffer);

  return 0;
}
