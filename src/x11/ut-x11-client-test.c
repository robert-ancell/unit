#include <assert.h>
#include <stdio.h>

#include "ut.h"

static size_t pixmap_width = 0;
static size_t pixmap_height = 0;

static UtObject *client = NULL;
static uint32_t wm_protocols_atom = 0;
static uint32_t wm_delete_window_atom = 0;
static uint32_t segment = 0;
static UtObject *buffer = NULL;
static uint32_t window = 0;
static uint32_t pixmap = 0;
static uint32_t gc = 0;

static void input_enter_cb(void *user_data, uint16_t device_id,
                           uint32_t timestamp, UtX11InputNotifyMode mode,
                           UtX11InputNotifyDetail detail, uint32_t window,
                           double x, double y) {
  printf("Enter\n");
}

static void input_leave_cb(void *user_data, uint16_t device_id,
                           uint32_t timestamp, UtX11InputNotifyMode mode,
                           UtX11InputNotifyDetail detail, uint32_t window,
                           double x, double y) {
  printf("Leave\n");
}

static void input_motion_cb(void *user_data, uint16_t device_id,
                            uint32_t timestamp, uint32_t window, double x,
                            double y, UtX11PointerEventFlag flag) {
  printf("Motion (%f,%f)\n", x, y);
}

static void input_button_press_cb(void *user_data, uint16_t device_id,
                                  uint32_t timestamp, uint32_t window,
                                  uint8_t button, double x, double y,
                                  UtX11PointerEventFlag flag) {
  printf("ButtonPress %d\n", button);
}

static void input_button_release_cb(void *user_data, uint16_t device_id,
                                    uint32_t timestamp, uint32_t window,
                                    uint8_t button, double x, double y,
                                    UtX11PointerEventFlag flag) {
  printf("ButtonRelease %d\n", button);
}

static void input_focus_in_cb(void *user_data, uint16_t device_id,
                              uint32_t timestamp, UtX11InputNotifyMode mode,
                              UtX11InputNotifyDetail detail, uint32_t window) {
  printf("FocusIn\n");
}

static void input_focus_out_cb(void *user_data, uint16_t device_id,
                               uint32_t timestamp, UtX11InputNotifyMode mode,
                               UtX11InputNotifyDetail detail, uint32_t window) {
  printf("FocusOut\n");
}

static void input_key_press_cb(void *user_data, uint16_t device_id,
                               uint32_t timestamp, uint32_t window,
                               uint8_t keycode, double x, double y,
                               UtX11KeyEventFlag flag) {
  printf("KeyPress %d\n", keycode);
}

static void input_key_release_cb(void *user_data, uint16_t device_id,
                                 uint32_t timestamp, uint32_t window,
                                 uint8_t keycode, double x, double y,
                                 UtX11KeyEventFlag flag) {
  printf("KeyRelease %d\n", keycode);
}

static void client_message_cb(void *user_data, uint32_t window, uint32_t type,
                              UtObject *data) {
  printf("ClientMessage\n");

  if (type == wm_protocols_atom) {
    if (ut_uint32_list_get_element(data, 0) == wm_delete_window_atom) {
      ut_event_loop_return(NULL);
    }
  }
}

static void configure_notify_cb(void *user_data, uint32_t window, int16_t x,
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

static void expose_cb(void *user_data, uint32_t window, uint16_t x, uint16_t y,
                      uint16_t width, uint16_t height, uint16_t count) {
  printf("Expose\n");
  ut_x11_client_copy_area(client, pixmap, window, gc, x, y, x, y, width,
                          height);
}

static UtX11EventCallbacks event_callbacks = {
    .input_enter = input_enter_cb,
    .input_leave = input_leave_cb,
    .input_motion = input_motion_cb,
    .input_button_press = input_button_press_cb,
    .input_button_release = input_button_release_cb,
    .input_focus_in = input_focus_in_cb,
    .input_focus_out = input_focus_out_cb,
    .input_key_press = input_key_press_cb,
    .input_key_release = input_key_release_cb,
    .client_message = client_message_cb,
    .configure_notify = configure_notify_cb,
    .expose = expose_cb};

static void error_cb(void *user_data, UtObject *error) {
  ut_cstring_ref s = ut_object_to_string(error);
  printf("%s\n", s);
}

static void list_extensions_cb(void *user_data, UtObject *names,
                               UtObject *error) {
  assert(error == NULL);
  size_t names_length = ut_list_get_length(names);
  for (size_t i = 0; i < names_length; i++) {
    printf("%s\n", ut_string_list_get_element(names, i));
  }
}

static void query_device_cb(void *user_data, UtObject *infos, UtObject *error) {
  size_t infos_length = ut_list_get_length(infos);
  for (size_t i = 0; i < infos_length; i++) {
    UtObjectRef info = ut_list_get_element(infos, i);
    printf("%s\n", ut_x11_input_device_info_get_name(info));
  }
}

static void wm_protocols_atom_cb(void *user_data, uint32_t atom,
                                 UtObject *error) {
  assert(error == NULL);
  wm_protocols_atom = atom;
}

static void wm_delete_window_atom_cb(void *user_data, uint32_t atom,
                                     UtObject *error) {
  assert(error == NULL);
  wm_delete_window_atom = atom;

  UtObjectRef protocols =
      ut_uint32_list_new_from_elements(1, wm_delete_window_atom);
  ut_x11_client_change_property_uint32(client, window, wm_protocols_atom,
                                       UT_X11_PROPERTY_MODE_REPLACE,
                                       UT_X11_ATOM, protocols);
}

static void connect_cb(void *user_data, UtObject *error) {
  if (error != NULL) {
    ut_cstring_ref description = ut_error_get_description(error);
    printf("Error connecting: %s", description);
    return;
  }

  printf("Connected\n");

  ut_x11_client_list_extensions(client, list_extensions_cb, NULL, NULL);

  ut_x11_client_query_device(client, UT_X11_DEVICE_ALL_MASTER, query_device_cb,
                             NULL, NULL);

  ut_x11_client_intern_atom(client, "WM_PROTOCOLS", false, wm_protocols_atom_cb,
                            NULL, NULL);
  ut_x11_client_intern_atom(client, "WM_DELETE_WINDOW", false,
                            wm_delete_window_atom_cb, NULL, NULL);

  window = ut_x11_client_create_window(client, 0, 0, 640, 480,
                                       UT_X11_EVENT_STRUCTURE_NOTIFY |
                                           UT_X11_EVENT_EXPOSURE);

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

  ut_x11_client_change_property_string(
      client, window, UT_X11_PROPERTY_MODE_REPLACE, UT_X11_WM_NAME, "UT");

  ut_x11_client_map_window(client, window);
}

int main(int argc, char **argv) {
  client = ut_x11_client_new(&event_callbacks, error_cb, NULL, NULL);
  ut_x11_client_connect(client, connect_cb, NULL, NULL);

  ut_event_loop_run();

  return 0;
}
