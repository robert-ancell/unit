#include <assert.h>
#include <stdio.h>

#include "ut.h"

static size_t width = 640;
static size_t height = 480;

static UtObject *client = NULL;
static uint32_t segment = 0;
static UtObject *buffer = NULL;
static uint32_t window = 0;
static uint32_t pixmap = 0;
static uint32_t gc = 0;

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

static void expose_cb(void *user_data, uint32_t window, uint16_t x, uint16_t y,
                      uint16_t width, uint16_t height) {
  ut_x11_client_copy_area(client, pixmap, window, gc, 0, 0, 0, 0, width,
                          height);
}

static UtX11EventCallbacks event_callbacks = {.enter_notify = enter_notify_cb,
                                              .leave_notify = leave_notify_cb,
                                              .motion_notify = motion_notify_cb,
                                              .button_press = button_press_cb,
                                              .button_release =
                                                  button_release_cb,
                                              .focus_in = focus_in_cb,
                                              .focus_out = focus_out_cb,
                                              .key_press = key_press_cb,
                                              .key_release = key_release_cb,
                                              .expose = expose_cb};

static void error_cb(void *user_data, UtObject *error) {
  ut_cstring_ref s = ut_object_to_string(error);
  printf("%s\n", s);
}

static void intern_atom_cb(void *user_data, uint32_t atom, UtObject *error) {
  printf("Atom %08x\n", atom);
}

static void get_atom_name_cb(void *user_data, const char *name,
                             UtObject *error) {
  printf("Atom name \"%s\"\n", name);
}

static void list_extensions_cb(void *user_data, UtObject *names,
                               UtObject *error) {
  size_t names_length = ut_list_get_length(names);
  for (size_t i = 0; i < names_length; i++) {
    printf("%s\n", ut_string_list_get_element(names, i));
  }
}

static void connect_cb(void *user_data, UtObject *error) {
  if (error != NULL) {
    ut_cstring_ref description = ut_error_get_description(error);
    printf("Error connecting: %s", description);
    return;
  }

  printf("Connected\n");

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

  ut_x11_client_intern_atom(client, "HELLO-WORLD", true, intern_atom_cb, NULL,
                            NULL);

  ut_x11_client_get_atom_name(client, 0x00000001, get_atom_name_cb, NULL, NULL);

  ut_x11_client_list_extensions(client, list_extensions_cb, NULL, NULL);

  window = ut_x11_client_create_window(
      client, 0, 0, width, height,
      UT_X11_EVENT_KEY_PRESS | UT_X11_EVENT_KEY_RELEASE |
          UT_X11_EVENT_BUTTON_PRESS | UT_X11_EVENT_BUTTON_RELEASE |
          UT_X11_EVENT_ENTER_WINDOW | UT_X11_EVENT_LEAVE_WINDOW |
          UT_X11_EVENT_POINTER_MOTION | UT_X11_EVENT_EXPOSURE |
          UT_X11_EVENT_FOCUS_CHANGE);

  UtObject *present = ut_x11_client_get_present_extension(client);
  ut_x11_present_extension_select_input(
      present, window,
      UT_X11_PRESENT_CONFIGURE_NOTIFY_MASK |
          UT_X11_PRESENT_COMPLETE_NOTIFY_MASK |
          UT_X11_PRESENT_IDLE_NOTIFY_MASK);

  ut_x11_client_map_window(client, window);

  UtObject *shm = ut_x11_client_get_mit_shm_extension(client);
  segment = ut_x11_mit_shm_extension_attach_fd(
      shm, ut_shared_memory_array_get_fd(buffer), false);
  pixmap = ut_x11_mit_shm_extension_create_pixmap(shm, window, width, height,
                                                  24, segment, 0);
  gc = ut_x11_client_create_gc(client, pixmap);
}

int main(int argc, char **argv) {
  client = ut_x11_client_new(&event_callbacks, error_cb, NULL, NULL);
  ut_x11_client_connect(client, connect_cb, NULL, NULL);

  ut_event_loop_run();

  return 0;
}
