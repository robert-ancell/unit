#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "ut.h"

static size_t width = 640;
static size_t height = 480;

static UtObject *client = NULL;
static UtObject *registry = NULL;
static UtObject *compositor = NULL;
static UtObject *shm = NULL;
static UtObject *seat = NULL;
static UtObject *pointer = NULL;
static UtObject *keyboard = NULL;
static UtObject *touch = NULL;
static UtObject *wm_base = NULL;
static UtObject *surface = NULL;
static UtObject *xdg_surface = NULL;

static void release_cb(UtObject *object) {}

static UtObject *draw_frame(UtObject *object) {
  UtObjectRef pool_data = ut_shared_memory_array_new(width * height * 4);
  uint8_t *pixel_data = ut_shared_memory_array_get_data(pool_data);
  for (size_t y = 0; y < height; y++) {
    for (size_t x = 0; x < width; x++) {
      uint8_t *pixel = pixel_data + (y * width * 4) + (x * 4);
      pixel[0] = 255 * x / width;
      pixel[1] = 255 * y / height;
      pixel[2] = 255;
      pixel[3] = 255;
    }
  }

  uint32_t format = UT_WAYLAND_SHM_FORMAT_argb8888;

  assert(ut_wayland_shm_has_format(shm, format));
  UtObjectRef pool =
      ut_wayland_shm_create_pool(shm, ut_shared_memory_array_get_fd(pool_data),
                                 ut_list_get_length(pool_data));
  UtObjectRef buffer = ut_wayland_shm_pool_create_buffer(
      pool, 0, width, height, width * 4, format, object, release_cb);
  ut_wayland_shm_pool_destroy(pool);

  return ut_object_ref(buffer);
}

static void configure_cb(UtObject *object, uint32_t serial) {
  ut_xdg_surface_ack_configure(xdg_surface, serial);
  UtObjectRef buffer = draw_frame(object);
  ut_wayland_surface_attach(surface, buffer, 0, 0);
  ut_wayland_surface_commit(surface);
}

static UtXdgSurfaceCallbacks xdg_surface_callbacks = {.configure =
                                                          configure_cb};

static void pointer_enter_cb(UtObject *object, uint32_t serial,
                             UtObject *surface, double surface_x,
                             double surface_y) {
  printf("wl_pointer::enter (%f,%f)\n", surface_x, surface_y);
}

static void pointer_leave_cb(UtObject *object, uint32_t serial,
                             UtObject *surface) {
  printf("wl_pointer::leave\n");
}

static void pointer_motion_cb(UtObject *object, uint32_t time, double surface_x,
                              double surface_y) {
  printf("wl_pointer::motion (%f,%f)\n", surface_x, surface_y);
}

static void pointer_button_cb(UtObject *object, uint32_t serial, uint32_t time,
                              uint32_t button, uint32_t state) {
  printf("wl_pointer::button %d %d\n", button, state);
}

static UtWaylandPointerCallbacks pointer_callbacks = {
    .enter = pointer_enter_cb,
    .leave = pointer_leave_cb,
    .motion = pointer_motion_cb,
    .button = pointer_button_cb};

static void keyboard_enter_cb(UtObject *object, uint32_t serial,
                              UtObject *surface, UtObject *keys) {
  ut_cstring_ref keys_string = ut_object_to_string(keys);
  printf("wl_keyboard::enter %s\n", keys_string);
}

static void keyboard_leave_cb(UtObject *object, uint32_t serial,
                              UtObject *surface) {
  printf("wl_keyboard::leave\n");
}

static void keyboard_key_cb(UtObject *object, uint32_t serial, uint32_t time,
                            uint32_t key, uint32_t state) {
  printf("wl_keyboard::key %d %d\n", key, state);
}

static void keyboard_modifiers_cb(UtObject *object, uint32_t serial,
                                  uint32_t mods_depressed,
                                  uint32_t mods_latched, uint32_t mods_locked,
                                  uint32_t group) {
  printf("wl_keyboard::modifiers %d %d %d %d\n", mods_depressed, mods_latched,
         mods_locked, group);
}

static UtWaylandKeyboardCallbacks keyboard_callbacks = {
    .enter = keyboard_enter_cb,
    .leave = keyboard_leave_cb,
    .key = keyboard_key_cb,
    .modifiers = keyboard_modifiers_cb};

static void touch_down_cb(UtObject *object, uint32_t serial, uint32_t time,
                          UtObject *surface, int32_t id, double surface_x,
                          double surface_y) {
  printf("wl_touch::down %d (%f,%f)\n", id, surface_x, surface_y);
}

static void touch_up_cb(UtObject *object, uint32_t serial, uint32_t time,
                        int32_t id) {
  printf("wl_touch::up %d\n", id);
}

static void touch_motion_cb(UtObject *object, uint32_t time, int32_t id,
                            double surface_x, double surface_y) {
  printf("wl_touch::motion %d (%f,%f)\n", id, surface_x, surface_y);
}

static UtWaylandTouchCallbacks touch_callbacks = {
    .down = touch_down_cb, .up = touch_up_cb, .motion = touch_motion_cb};

static void seat_capabilities_cb(UtObject *object, uint32_t capabilities) {
  if ((capabilities & UT_WAYLAND_SEAT_CAPABILITY_POINTER) != 0) {
    UtObjectRef p =
        ut_wayland_seat_get_pointer(seat, object, &pointer_callbacks);
    ut_object_set(&pointer, p);
  } else {
    ut_object_clear(&pointer);
  }

  if ((capabilities & UT_WAYLAND_SEAT_CAPABILITY_KEYBOARD) != 0) {
    UtObjectRef k =
        ut_wayland_seat_get_keyboard(seat, object, &keyboard_callbacks);
    ut_object_set(&keyboard, k);
  } else {
    ut_object_clear(&keyboard);
  }

  if ((capabilities & UT_WAYLAND_SEAT_CAPABILITY_TOUCH) != 0) {
    ut_object_set(&touch,
                  ut_wayland_seat_get_touch(seat, object, &touch_callbacks));
  } else {
    ut_object_clear(&touch);
  }
}

static UtWaylandSeatCallbacks seat_callbacks = {.capabilities =
                                                    seat_capabilities_cb};

static void toplevel_close_cb(UtObject *object) { ut_event_loop_return(NULL); }

static UtXdgToplevelCallbacks toplevel_callbacks = {.close = toplevel_close_cb};

static UtWaylandSurfaceCallbacks surface_callbacks = {};

static void sync_cb(UtObject *object, uint32_t callback_data) {
  printf("Connected\n");

  surface = ut_wayland_compositor_create_surface(compositor, object,
                                                 &surface_callbacks);

  xdg_surface = ut_xdg_wm_base_get_xdg_surface(wm_base, surface, object,
                                               &xdg_surface_callbacks);
  UtObjectRef toplevel =
      ut_xdg_surface_get_toplevel(xdg_surface, object, &toplevel_callbacks);
  ut_xdg_toplevel_set_title(toplevel, "Test Window");
  ut_wayland_surface_commit(surface);
}

static void ping_cb(UtObject *object, uint32_t serial) {
  ut_xdg_wm_base_pong(wm_base, serial);
}

static UtXdgWmBaseCallbacks wm_base_callbacks = {.ping = ping_cb};

static void global_cb(UtObject *object, uint32_t name, const char *interface,
                      uint32_t version) {
  printf("%d %s %d\n", name, interface, version);
  if (strcmp(interface, "wl_compositor") == 0) {
    compositor = ut_wayland_compositor_new_from_registry(registry, name);
  } else if (strcmp(interface, "wl_shm") == 0) {
    shm = ut_wayland_shm_new_from_registry(registry, name);
  } else if (strcmp(interface, "wl_seat") == 0) {
    seat = ut_wayland_seat_new_from_registry(registry, name, object,
                                             &seat_callbacks);
  } else if (strcmp(interface, "xdg_wm_base") == 0) {
    wm_base = ut_xdg_wm_base_new_from_registry(registry, name, object,
                                               &wm_base_callbacks);
  }
}

static UtWaylandRegistryCallbacks registry_callbacks = {.global = global_cb};

int main(int argc, char **argv) {
  client = ut_wayland_client_new();
  ut_wayland_client_connect(client);

  UtObjectRef dummy_object = ut_null_new();
  registry =
      ut_wayland_client_get_registry(client, dummy_object, &registry_callbacks);
  UtObjectRef callback = ut_wayland_client_sync(client, dummy_object, sync_cb);

  ut_event_loop_run();

  ut_object_unref(registry);
  ut_object_unref(compositor);
  ut_object_unref(shm);
  ut_object_unref(seat);
  ut_object_unref(pointer);
  ut_object_unref(keyboard);
  ut_object_unref(touch);
  ut_object_unref(wm_base);
  ut_object_unref(surface);
  ut_object_unref(xdg_surface);
  ut_object_unref(client);

  return 0;
}
