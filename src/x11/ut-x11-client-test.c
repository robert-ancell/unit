#include <assert.h>
#include <stdio.h>

#include "ut.h"

static size_t pixmap_width = 0;
static size_t pixmap_height = 0;

static UtObject *client = NULL;
static uint32_t wm_protocols_atom = 0;
static uint32_t wm_delete_window_atom = 0;
static uint32_t wm_sync_request_atom = 0;
static uint32_t wm_sync_request_counter_atom = 0;
static uint32_t segment = 0;
static UtObject *buffer = NULL;
static uint32_t window = 0;
static uint32_t pixmap = 0;
static uint32_t gc = 0;
static uint32_t sync_counter = 0;
static uint32_t extended_sync_counter = 0;
static bool sync_counter_value_is_extended = false;
static int64_t sync_counter_value = 0;

static void enter_notify_cb(void *user_data, uint32_t window, int16_t x,
                            int16_t y) {
  printf("EnterNotify\n");
}

static void leave_notify_cb(void *user_data, uint32_t window, int16_t x,
                            int16_t y) {
  printf("LeaveNotify\n");
}

static void motion_notify_cb(void *user_data, uint32_t window, int16_t x,
                             int16_t y) {
  printf("MotionNotify (%d,%d)\n", x, y);
}

static void button_press_cb(void *user_data, uint32_t window, uint8_t button,
                            int16_t x, int16_t y) {
  printf("ButtonPress %d\n", button);
}

static void button_release_cb(void *user_data, uint32_t window, uint8_t button,
                              int16_t x, int16_t y) {
  printf("ButtonRelease %d\n", button);
}

static void focus_in_cb(void *user_data, uint32_t window) {
  printf("FocusIn\n");
}

static void focus_out_cb(void *user_data, uint32_t window) {
  printf("FocusOut\n");
}

static void key_press_cb(void *user_datao, uint32_t window, uint8_t keycode,
                         int16_t x, int16_t y) {
  printf("KeyPress %d\n", keycode);
}

static void key_release_cb(void *user_data, uint32_t window, uint8_t keycode,
                           int16_t x, int16_t y) {
  printf("KeyRelease %d\n", keycode);
}

static void client_message_cb(void *user_data, uint32_t window, uint32_t type,
                              UtObject *data) {
  printf("ClientMessage\n");

  if (type == wm_protocols_atom) {
    uint32_t protocol = ut_uint32_list_get_element(data, 0);
    if (protocol == wm_delete_window_atom) {
      ut_event_loop_return(NULL);
    } else if (protocol == wm_sync_request_atom) {
      uint64_t counter_value_low = ut_uint32_list_get_element(data, 2);
      uint64_t counter_value_high = ut_uint32_list_get_element(data, 3);
      sync_counter_value_is_extended = ut_uint32_list_get_element(data, 4) != 0;
      sync_counter_value =
          (int64_t)(counter_value_high << 32 | counter_value_low);
      printf("%s %d %li\n", ut_object_to_string(data),
             sync_counter_value_is_extended, sync_counter_value);
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

  ut_x11_client_set_counter(client, extended_sync_counter, sync_counter_value);
}

static UtX11EventCallbacks event_callbacks = {
    .enter_notify = enter_notify_cb,
    .leave_notify = leave_notify_cb,
    .motion_notify = motion_notify_cb,
    .button_press = button_press_cb,
    .button_release = button_release_cb,
    .focus_in = focus_in_cb,
    .focus_out = focus_out_cb,
    .key_press = key_press_cb,
    .key_release = key_release_cb,
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

static void wm_protocols_atom_cb(void *user_data, uint32_t atom,
                                 UtObject *error) {
  assert(error == NULL);
  wm_protocols_atom = atom;
}

static void wm_delete_window_atom_cb(void *user_data, uint32_t atom,
                                     UtObject *error) {
  assert(error == NULL);
  wm_delete_window_atom = atom;
}

static void wm_sync_request_atom_cb(void *user_data, uint32_t atom,
                                    UtObject *error) {
  assert(error == NULL);
  wm_sync_request_atom = atom;
}

static void wm_sync_request_counter_atom_cb(void *user_data, uint32_t atom,
                                            UtObject *error) {
  assert(error == NULL);
  wm_sync_request_counter_atom = atom;

  // We support window deletion requests and paint synchronization.
  UtObjectRef protocols = ut_uint32_list_new();
  ut_uint32_list_append(protocols, wm_delete_window_atom);
  ut_uint32_list_append(protocols, wm_sync_request_atom);
  ut_x11_client_change_property_uint32(client, window, wm_protocols_atom,
                                       UT_X11_ATOM, protocols);

  UtObjectRef counter_value = ut_uint32_list_new();
  ut_uint32_list_append(counter_value, sync_counter);
  ut_uint32_list_append(counter_value, extended_sync_counter);
  ut_x11_client_change_property_uint32(client, window,
                                       wm_sync_request_counter_atom,
                                       UT_X11_CARDINAL, counter_value);
}

static void connect_cb(void *user_data, UtObject *error) {
  if (error != NULL) {
    ut_cstring_ref description = ut_error_get_description(error);
    printf("Error connecting: %s", description);
    return;
  }

  printf("Connected\n");

  ut_x11_client_list_extensions(client, list_extensions_cb, NULL, NULL);

  ut_x11_client_intern_atom(client, "WM_PROTOCOLS", false, wm_protocols_atom_cb,
                            NULL, NULL);
  ut_x11_client_intern_atom(client, "WM_DELETE_WINDOW", false,
                            wm_delete_window_atom_cb, NULL, NULL);
  ut_x11_client_intern_atom(client, "_NET_WM_SYNC_REQUEST", false,
                            wm_sync_request_atom_cb, NULL, NULL);
  ut_x11_client_intern_atom(client, "_NET_WM_SYNC_REQUEST_COUNTER", false,
                            wm_sync_request_counter_atom_cb, NULL, NULL);

  window = ut_x11_client_create_window(
      client, 0, 0, 640, 480,
      UT_X11_EVENT_KEY_PRESS | UT_X11_EVENT_KEY_RELEASE |
          UT_X11_EVENT_BUTTON_PRESS | UT_X11_EVENT_BUTTON_RELEASE |
          UT_X11_EVENT_ENTER_WINDOW | UT_X11_EVENT_LEAVE_WINDOW |
          UT_X11_EVENT_POINTER_MOTION | UT_X11_EVENT_STRUCTURE_NOTIFY |
          UT_X11_EVENT_EXPOSURE | UT_X11_EVENT_FOCUS_CHANGE);
  sync_counter = ut_x11_client_create_counter(client, 0);
  extended_sync_counter = ut_x11_client_create_counter(client, 0);

  ut_x11_client_change_property_string(client, window, UT_X11_WM_NAME, "UT");

  ut_x11_client_map_window(client, window);
}

int main(int argc, char **argv) {
  client = ut_x11_client_new(&event_callbacks, error_cb, NULL, NULL);
  ut_x11_client_connect(client, connect_cb, NULL, NULL);

  ut_event_loop_run();

  return 0;
}
