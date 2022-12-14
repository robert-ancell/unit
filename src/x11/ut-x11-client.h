#include <stdbool.h>

#include "ut-object.h"

#pragma once

#define UT_X11_PRIMARY 1
#define UT_X11_SECONDARY 2
#define UT_X11_ARC 3
#define UT_X11_ATOM 4
#define UT_X11_BITMAP 5
#define UT_X11_CARDINAL 6
#define UT_X11_COLORMAP 7
#define UT_X11_CURSOR 8
#define UT_X11_CUT_BUFFER0 8
#define UT_X11_CUT_BUFFER1 9
#define UT_X11_CUT_BUFFER2 10
#define UT_X11_CUT_BUFFER3 11
#define UT_X11_CUT_BUFFER4 12
#define UT_X11_CUT_BUFFER5 13
#define UT_X11_CUT_BUFFER6 14
#define UT_X11_CUT_BUFFER7 16
#define UT_X11_DRAWABLE 17
#define UT_X11_FONT 18
#define UT_X11_INTEGER 19
#define UT_X11_PIXMAP 20
#define UT_X11_POINT 21
#define UT_X11_RECTANGLE 22
#define UT_X11_RESOURCE_MANAGER 23
#define UT_X11_RGB_COLOR_MAP 24
#define UT_X11_RGB_BEST_MAP 25
#define UT_X11_RGB_BLUE_MAP 26
#define UT_X11_RGB_DEFAULT_MAP 27
#define UT_X11_RGB_GRAY_MAP 28
#define UT_X11_RGB_GREEN_MAP 29
#define UT_X11_RGB_RED_MAP 30
#define UT_X11_STRING 31
#define UT_X11_VISUALID 32
#define UT_X11_WINDOW 33
#define UT_X11_WM_COMMAND 34
#define UT_X11_WM_HINTS 35
#define UT_X11_WM_CLIENT_MACHINE 36
#define UT_X11_WM_ICON_NAME 37
#define UT_X11_WM_ICON_SIZE 38
#define UT_X11_WM_NAME 39
#define UT_X11_WM_NORMAL_HINTS 40
#define UT_X11_WM_SIZE_HINTS 41
#define UT_X11_WM_ZOOM_HINTS 42
#define UT_X11_MIN_SPACE 43
#define UT_X11_NORM_SPACE 44
#define UT_X11_MAX_SPACE 45
#define UT_X11_END_SPACE 46
#define UT_X11_SUPERSCRIPT_X 47
#define UT_X11_SUPERSCRIPT_Y 48
#define UT_X11_SUBSCRIPT_X 49
#define UT_X11_SUBSCRIPT_Y 50
#define UT_X11_UNDERLINE_POSITION 51
#define UT_X11_UNDERLINE_THICKNESS 52
#define UT_X11_STRIKEOUT_ASCENT 53
#define UT_X11_STRIKEOUT_DESCENT 54
#define UT_X11_ITALIC_ANGLE 55
#define UT_X11_X_HEIGHT 56
#define UT_X11_QUAD_WIDTH 57
#define UT_X11_WEIGHT 58
#define UT_X11_POINT_SIZE 59
#define UT_X11_RESOLUTION 60
#define UT_X11_COPYRIGHT 61
#define UT_X11_NOTICE 62
#define UT_X11_FONT_NAME 63
#define UT_X11_FAMILY_NAME 64
#define UT_X11_FULL_NAME 65
#define UT_X11_CAP_HEIGHT 66
#define UT_X11_WM_CLASS 67
#define UT_X11_WM_TRANSIENT_FOR 68

typedef void (*UtX11KeyPressCallback)(void *user_data, uint32_t window,
                                      uint8_t keycode, int16_t x, int16_t y);
typedef void (*UtX11KeyReleaseCallback)(void *user_data, uint32_t window,
                                        uint8_t keycode, int16_t x, int16_t y);
typedef void (*UtX11ButtonPressCallback)(void *user_data, uint32_t window,
                                         uint8_t button, int16_t x, int16_t y);
typedef void (*UtX11ButtonReleaseCallback)(void *user_data, uint32_t window,
                                           uint8_t button, int16_t x,
                                           int16_t y);
typedef void (*UtX11MotionNotifyCallback)(void *user_data, uint32_t window,
                                          int16_t x, int16_t y);
typedef void (*UtX11EnterNotifyCallback)(void *user_data, uint32_t window,
                                         int16_t x, int16_t y);
typedef void (*UtX11LeaveNotifyCallback)(void *user_data, uint32_t window,
                                         int16_t x, int16_t y);
typedef void (*UtX11FocusInCallback)(void *user_data, uint32_t window);
typedef void (*UtX11FocusOutCallback)(void *user_data, uint32_t window);
typedef void (*UtX11ExposeCallback)(void *user_data, uint32_t window,
                                    uint16_t x, uint16_t y, uint16_t width,
                                    uint16_t height, uint16_t count);
typedef void (*UtX11NoExposeCallback)(void *user_data, uint32_t drawable);
typedef void (*UtX11MapNotifyCallback)(void *user_data, uint32_t event,
                                       uint32_t window, bool override_redirect);
typedef void (*UtX11ReparentNotifyCallback)(void *user_data, int32_t event,
                                            uint32_t window, uint32_t parent,
                                            int16_t x, int16_t y,
                                            bool override_redirect);
typedef void (*UtX11ConfigureNotifyCallback)(void *user_data, uint32_t window,
                                             int16_t x, int16_t y,
                                             uint16_t width, uint16_t height);
typedef void (*UtX11PropertyNotifyCallback)(void *user_data, uint32_t window,
                                            uint32_t atom);
typedef void (*UtX11ClientMessageCallback)(void *user_data, uint32_t window,
                                           uint32_t type, UtObject *data);

// From XFIXES
typedef void (*UtX11SelectionNotifyCallback)(void *user_data, uint32_t window,
                                             uint32_t owner, uint32_t selection,
                                             uint32_t timestamp,
                                             uint32_t selection_timestamp);
typedef void (*UtX11CursorNotifyCallback)(void *user_data, uint32_t window,
                                          uint32_t cursor_serial,
                                          uint32_t timestamp, uint32_t name);

// From PRESENT
typedef void (*UtX11PresentConfigureNotifyCallback)(
    void *user_data, uint32_t event_id, uint32_t window, int16_t x, int16_t y,
    uint16_t width, uint16_t height, int16_t off_x, int16_t off_y,
    uint16_t pixmap_width, uint16_t pixmap_height, uint32_t pixmap_flags);
typedef void (*UtX11PresentCompleteNotifyCallback)(
    void *user_data, uint8_t kind, uint8_t mode, uint32_t event_id,
    uint32_t window, uint32_t serial, uint64_t ust, uint64_t msc);
typedef void (*UtX11PresentIdleNotifyCallback)(void *user_data,
                                               uint32_t event_id,
                                               uint32_t window, uint32_t serial,
                                               uint32_t pixmap,
                                               uint32_t idle_fence);

typedef void (*UtX11UnknownEventCallback)(void *user_data, uint8_t code,
                                          bool from_send_event,
                                          uint16_t sequence_number,
                                          uint8_t data0, UtObject *data);
typedef void (*UtX11UnknownGenericEventCallback)(void *user_data,
                                                 uint8_t major_opcode,
                                                 uint16_t code);

typedef struct {
  UtX11KeyPressCallback key_press;
  UtX11KeyReleaseCallback key_release;
  UtX11ButtonPressCallback button_press;
  UtX11ButtonReleaseCallback button_release;
  UtX11MotionNotifyCallback motion_notify;
  UtX11EnterNotifyCallback enter_notify;
  UtX11LeaveNotifyCallback leave_notify;
  UtX11FocusInCallback focus_in;
  UtX11FocusOutCallback focus_out;
  UtX11ExposeCallback expose;
  UtX11NoExposeCallback no_expose;
  UtX11MapNotifyCallback map_notify;
  UtX11ReparentNotifyCallback reparent_notify;
  UtX11ConfigureNotifyCallback configure_notify;
  UtX11PropertyNotifyCallback property_notify;
  UtX11ClientMessageCallback client_message;

  // From XFIXES
  UtX11SelectionNotifyCallback selection_notify;
  UtX11CursorNotifyCallback cursor_notify;

  // From PRESENT
  UtX11PresentConfigureNotifyCallback present_configure_notify;
  UtX11PresentCompleteNotifyCallback present_complete_notify;
  UtX11PresentIdleNotifyCallback present_idle_notify;

  UtX11UnknownEventCallback unknown_event;
  UtX11UnknownGenericEventCallback unknown_generic_event;
} UtX11EventCallbacks;

typedef void (*UtX11ClientErrorCallback)(void *user_data, UtObject *error);
typedef void (*UtX11ClientConnectCallback)(void *user_data, UtObject *error);
typedef void (*UtX11InternAtomCallback)(void *user_data, uint32_t atom,
                                        UtObject *error);
typedef void (*UtX11GetAtomNameCallback)(void *user_data, const char *name,
                                         UtObject *error);
typedef void (*UtX11GetPropertyCallback)(void *user_data, uint32_t type,
                                         UtObject *value, uint32_t bytes_after,
                                         UtObject *error);
typedef void (*UtX11ListPropertiesCallback)(void *user_data, UtObject *atoms,
                                            UtObject *error);
typedef void (*UtX11QueryExtensionCallback)(void *user_data, bool present,
                                            uint8_t major_opcode,
                                            uint8_t first_event,
                                            uint8_t first_error,
                                            UtObject *error);
typedef void (*UtX11ListExtensionsCallback)(void *user_data, UtObject *names,
                                            UtObject *error);
typedef void (*UtX11ShmCreateSegmentCallback)(void *user_data, UtObject *fd,
                                              UtObject *error);
typedef void (*UtX11ClientListSystemCountersCallback)(void *user_data,
                                                      UtObject *counters,
                                                      UtObject *error);
typedef void (*UtX11ClientQueryCounterCallback)(void *user_data,
                                                int64_t counter_value,
                                                UtObject *error);

typedef enum {
  UT_X11_EVENT_KEY_PRESS = 0x00000001,
  UT_X11_EVENT_KEY_RELEASE = 0x00000002,
  UT_X11_EVENT_BUTTON_PRESS = 0x00000004,
  UT_X11_EVENT_BUTTON_RELEASE = 0x00000008,
  UT_X11_EVENT_ENTER_WINDOW = 0x00000010,
  UT_X11_EVENT_LEAVE_WINDOW = 0x00000020,
  UT_X11_EVENT_POINTER_MOTION = 0x00000040,
  UT_X11_EVENT_POINTER_MOTION_HINT = 0x00000080,
  UT_X11_EVENT_BUTTON1_MOTION = 0x00000100,
  UT_X11_EVENT_BUTTON2_MOTION = 0x00000200,
  UT_X11_EVENT_BUTTON3_MOTION = 0x00000400,
  UT_X11_EVENT_BUTTON4_MOTION = 0x00000800,
  UT_X11_EVENT_BUTTON5_MOTION = 0x00001000,
  UT_X11_EVENT_BUTTON_MOTION = 0x00002000,
  UT_X11_EVENT_KEYMAP_STATE = 0x00004000,
  UT_X11_EVENT_EXPOSURE = 0x00008000,
  UT_X11_EVENT_VISIBILITY_CHANGE = 0x00010000,
  UT_X11_EVENT_STRUCTURE_NOTIFY = 0x00020000,
  UT_X11_EVENT_RESIZE_REDIRECT = 0x00040000,
  UT_X11_EVENT_SUBSTRUCTURE_NOTIFY = 0x00080000,
  UT_X11_EVENT_SUBSTRUCTURE_REDIRECT = 0x00100000,
  UT_X11_EVENT_FOCUS_CHANGE = 0x00200000,
  UT_X11_EVENT_PROPERTY_CHANGE = 0x00400000,
  UT_X11_EVENT_COLORMAP_CHANGE = 0x00800000,
  UT_X11_EVENT_OWNER_GRAB_BUTTON = 0x01000000
} UtX11Event;

typedef enum {
  UT_X11_IMAGE_FORMAT_BITMAP,
  UT_X11_IMAGE_FORMAT_XY_PIXMAP,
  UT_X11_IMAGE_FORMAT_Z_PIXMAP
} UtX11ImageFormat;

typedef enum {
  UT_X11_PROPERTY_MODE_REPLACE = 0,
  UT_X11_PROPERTY_MODE_PREPEND = 1,
  UT_X11_PROPERTY_MODE_APPEND = 2
} UtX11PropertyMode;

UtObject *ut_x11_client_new(const UtX11EventCallbacks *event_callbacks,
                            UtX11ClientErrorCallback error_callback,
                            void *user_data, UtObject *cancel);

void ut_x11_client_connect(UtObject *object,
                           UtX11ClientConnectCallback callback, void *user_data,
                           UtObject *cancel);

const char *ut_x11_client_get_vendor(UtObject *object);

UtObject *ut_x11_client_get_pixmap_formats(UtObject *object);

UtObject *ut_x11_client_get_screens(UtObject *object);

uint32_t ut_x11_client_create_window(UtObject *object, int16_t x, int16_t y,
                                     uint16_t width, uint16_t height,
                                     uint32_t event_mask);

void ut_x11_client_destroy_window(UtObject *object, uint32_t window);

void ut_x11_client_map_window(UtObject *object, uint32_t window);

void ut_x11_client_unmap_window(UtObject *object, uint32_t window);

void ut_x11_client_configure_window(UtObject *object, uint32_t window,
                                    int16_t x, int16_t y, uint16_t width,
                                    uint16_t height);

void ut_x11_client_intern_atom(UtObject *object, const char *name,
                               bool only_if_exists,
                               UtX11InternAtomCallback callback,
                               void *user_data, UtObject *cancel);

void ut_x11_client_get_atom_name(UtObject *object, uint32_t atom,
                                 UtX11GetAtomNameCallback callback,
                                 void *user_data, UtObject *cancel);

void ut_x11_client_change_property_uint8(UtObject *object, uint32_t window,
                                         uint32_t property,
                                         UtX11PropertyMode mode, uint32_t type,
                                         UtObject *value);

void ut_x11_client_change_property_uint16(UtObject *object, uint32_t window,
                                          uint32_t property,
                                          UtX11PropertyMode mode, uint32_t type,
                                          UtObject *value);

void ut_x11_client_change_property_uint32(UtObject *object, uint32_t window,
                                          uint32_t property,
                                          UtX11PropertyMode mode, uint32_t type,
                                          UtObject *value);

void ut_x11_client_change_property_string(UtObject *object, uint32_t window,
                                          uint32_t property,
                                          UtX11PropertyMode mode,
                                          const char *value);

void ut_x11_client_delete_property(UtObject *object, uint32_t window,
                                   uint32_t property);

void ut_x11_client_get_property(UtObject *object, uint32_t window,
                                uint32_t property,
                                UtX11GetPropertyCallback callback,
                                void *user_data, UtObject *cancel);

void ut_x11_client_get_property_full(UtObject *object, uint32_t window,
                                     uint32_t property, uint32_t type,
                                     uint32_t long_offset, uint32_t long_length,
                                     bool delete,
                                     UtX11GetPropertyCallback callback,
                                     void *user_data, UtObject *cancel);

void ut_x11_client_list_properties(UtObject *object, uint32_t window,
                                   UtX11ListPropertiesCallback callback,
                                   void *user_data, UtObject *cancel);

uint32_t ut_x11_client_create_pixmap(UtObject *object, uint32_t drawable,
                                     uint16_t width, uint16_t height,
                                     uint8_t depth);

void ut_x11_client_free_pixmap(UtObject *object, uint32_t pixmap);

uint32_t ut_x11_client_create_gc(UtObject *object, uint32_t drawable);

void ut_x11_client_free_gc(UtObject *object, uint32_t gc);

void ut_x11_client_clear_area(UtObject *object, uint32_t window, int16_t x,
                              int16_t y, uint16_t width, uint16_t height,
                              bool exposures);

void ut_x11_client_copy_area(UtObject *object, uint32_t src_drawable,
                             uint32_t dst_drawable, uint32_t gc, int16_t src_x,
                             int16_t src_y, int16_t dst_x, int16_t dst_y,
                             uint16_t width, uint16_t height);

void ut_x11_client_put_image(UtObject *object, uint32_t drawable, uint32_t gc,
                             UtX11ImageFormat format, uint16_t width,
                             uint16_t height, uint8_t depth, int16_t dst_x,
                             int16_t dst_y, uint8_t *data, size_t data_length);

void ut_x11_client_query_extension(UtObject *object, const char *name,
                                   UtX11QueryExtensionCallback callback,
                                   void *user_data, UtObject *cancel);

void ut_x11_client_list_extensions(UtObject *object,
                                   UtX11ListExtensionsCallback callback,
                                   void *user_data, UtObject *cancel);

void ut_x11_client_bell(UtObject *object);

void ut_x11_client_kill_client(UtObject *object, uint32_t resource);

uint32_t ut_x11_client_shm_attach(UtObject *object, uint32_t shmid,
                                  bool read_only);

void ut_x11_client_shm_detach(UtObject *object, uint32_t segment);

uint32_t ut_x11_client_shm_create_pixmap(UtObject *object, uint32_t drawable,
                                         uint16_t width, uint16_t height,
                                         uint8_t depth, uint32_t segment,
                                         uint32_t offset);

uint32_t ut_x11_client_shm_attach_fd(UtObject *object, UtObject *fd,
                                     bool read_only);

uint32_t ut_x11_client_shm_create_segment(
    UtObject *object, uint32_t size, bool read_only,
    UtX11ShmCreateSegmentCallback callback, void *user_data, UtObject *cancel);

void ut_x11_client_list_system_counters(
    UtObject *object, UtX11ClientListSystemCountersCallback callback,
    void *user_data, UtObject *cancel);

uint32_t ut_x11_client_create_counter(UtObject *object, int64_t initial_value);

void ut_x11_client_set_counter(UtObject *object, uint32_t counter,
                               int64_t amount);

void ut_x11_client_change_counter(UtObject *object, uint32_t counter,
                                  int64_t amount);

void ut_x11_client_query_counter(UtObject *object, uint32_t counter,
                                 UtX11ClientQueryCounterCallback callback,
                                 void *user_data, UtObject *cancel);

void ut_x11_client_destroy_counter(UtObject *object, uint32_t counter);

bool ut_object_is_x11_client(UtObject *object);
