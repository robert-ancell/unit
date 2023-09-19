#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Predefined X11 atoms:
/// - [UT_X11_PRIMARY] - `PRIMARY` atom.
/// - [UT_X11_SECONDARY] - `UT_X11_SECONDARY` atom.
/// - [UT_X11_ARC] - `UT_X11_ARC` atom.
/// - [UT_X11_ATOM] - `UT_X11_ATOM` atom.
/// - [UT_X11_BITMAP] - `UT_X11_BITMAP` atom.
/// - [UT_X11_CARDINAL] - `UT_X11_CARDINAL` atom.
/// - [UT_X11_COLORMAP] - `UT_X11_COLORMAP` atom.
/// - [UT_X11_CURSOR] - `UT_X11_CURSOR` atom.
/// - [UT_X11_CUT_BUFFER0] - `UT_X11_CUT_BUFFER0` atom.
/// - [UT_X11_CUT_BUFFER1] - `UT_X11_CUT_BUFFER1` atom.
/// - [UT_X11_CUT_BUFFER2] - `UT_X11_CUT_BUFFER2` atom.
/// - [UT_X11_CUT_BUFFER3] - `UT_X11_CUT_BUFFER3` atom.
/// - [UT_X11_CUT_BUFFER4] - `UT_X11_CUT_BUFFER4` atom.
/// - [UT_X11_CUT_BUFFER5] - `UT_X11_CUT_BUFFER5` atom.
/// - [UT_X11_CUT_BUFFER6] - `UT_X11_CUT_BUFFER6` atom.
/// - [UT_X11_CUT_BUFFER7] - `UT_X11_CUT_BUFFER7` atom.
/// - [UT_X11_DRAWABLE] - `UT_X11_DRAWABLE` atom.
/// - [UT_X11_FONT] - `UT_X11_FONT` atom.
/// - [UT_X11_INTEGER] - `UT_X11_INTEGER` atom.
/// - [UT_X11_PIXMAP] - `UT_X11_PIXMAP` atom.
/// - [UT_X11_POINT] - `UT_X11_POINT` atom.
/// - [UT_X11_RECTANGLE] - `UT_X11_RECTANGLE` atom.
/// - [UT_X11_RESOURCE_MANAGER] - `UT_X11_RESOURCE_MANAGER` atom.
/// - [UT_X11_RGB_COLOR_MAP] - `UT_X11_RGB_COLOR_MAP` atom.
/// - [UT_X11_RGB_BEST_MAP] - `UT_X11_RGB_BEST_MAP` atom.
/// - [UT_X11_RGB_BLUE_MAP] - `UT_X11_RGB_BLUE_MAP` atom.
/// - [UT_X11_RGB_DEFAULT_MAP] - `UT_X11_RGB_DEFAULT_MAP` atom.
/// - [UT_X11_RGB_GRAY_MAP] - `UT_X11_RGB_GRAY_MAP` atom.
/// - [UT_X11_RGB_GREEN_MAP] - `UT_X11_RGB_GREEN_MAP` atom.
/// - [UT_X11_RGB_RED_MAP] - `UT_X11_RGB_RED_MAP` atom.
/// - [UT_X11_STRING] - `UT_X11_STRING` atom.
/// - [UT_X11_VISUALID] - `UT_X11_VISUALID` atom.
/// - [UT_X11_WINDOW] - `UT_X11_WINDOW` atom.
/// - [UT_X11_WM_COMMAND] - `UT_X11_WM_COMMAND` atom.
/// - [UT_X11_WM_HINTS] - `UT_X11_WM_HINTS` atom.
/// - [UT_X11_WM_CLIENT_MACHINE] - `UT_X11_WM_CLIENT_MACHINE` atom.
/// - [UT_X11_WM_ICON_NAME] - `UT_X11_WM_ICON_NAME` atom.
/// - [UT_X11_WM_ICON_SIZE] - `UT_X11_WM_ICON_SIZE` atom.
/// - [UT_X11_WM_NAME] - `UT_X11_WM_NAME` atom.
/// - [UT_X11_WM_NORMAL_HINTS] - `UT_X11_WM_NORMAL_HINTS` atom.
/// - [UT_X11_WM_SIZE_HINTS] - `UT_X11_WM_SIZE_HINTS` atom.
/// - [UT_X11_WM_ZOOM_HINTS] - `UT_X11_WM_ZOOM_HINTS` atom.
/// - [UT_X11_MIN_SPACE] - `UT_X11_MIN_SPACE` atom.
/// - [UT_X11_NORM_SPACE] - `UT_X11_NORM_SPACE` atom.
/// - [UT_X11_MAX_SPACE] - `UT_X11_MAX_SPACE` atom.
/// - [UT_X11_END_SPACE] - `UT_X11_END_SPACE` atom.
/// - [UT_X11_SUPERSCRIPT_X] - `UT_X11_SUPERSCRIPT_X` atom.
/// - [UT_X11_SUPERSCRIPT_Y] - `UT_X11_SUPERSCRIPT_Y` atom.
/// - [UT_X11_SUBSCRIPT_X] - `UT_X11_SUBSCRIPT_X` atom.
/// - [UT_X11_SUBSCRIPT_Y] - `UT_X11_SUBSCRIPT_Y` atom.
/// - [UT_X11_UNDERLINE_POSITION] - `UT_X11_UNDERLINE_POSITION` atom.
/// - [UT_X11_UNDERLINE_THICKNESS] - `UT_X11_UNDERLINE_THICKNESS` atom.
/// - [UT_X11_STRIKEOUT_ASCENT] - `UT_X11_STRIKEOUT_ASCENT` atom.
/// - [UT_X11_STRIKEOUT_DESCENT] - `UT_X11_STRIKEOUT_DESCENT` atom.
/// - [UT_X11_ITALIC_ANGLE] - `UT_X11_ITALIC_ANGLE` atom.
/// - [UT_X11_X_HEIGHT] - `UT_X11_X_HEIGHT` atom.
/// - [UT_X11_QUAD_WIDTH] - `UT_X11_QUAD_WIDTH` atom.
/// - [UT_X11_WEIGHT] - `UT_X11_WEIGHT` atom.
/// - [UT_X11_POINT_SIZE] - `UT_X11_POINT_SIZE` atom.
/// - [UT_X11_RESOLUTION] - `UT_X11_RESOLUTION` atom.
/// - [UT_X11_COPYRIGHT] - `UT_X11_COPYRIGHT` atom.
/// - [UT_X11_NOTICE] - `UT_X11_NOTICE` atom.
/// - [UT_X11_FONT_NAME] - `UT_X11_FONT_NAME` atom.
/// - [UT_X11_FAMILY_NAME] - `UT_X11_FAMILY_NAME` atom.
/// - [UT_X11_FULL_NAME] - `UT_X11_FULL_NAME` atom.
/// - [UT_X11_CAP_HEIGHT] - `UT_X11_CAP_HEIGHT` atom.
/// - [UT_X11_WM_CLASS] - `UT_X11_WM_CLASS` atom.
/// - [UT_X11_WM_TRANSIENT_FOR] - `UT_X11_WM_TRANSIENT_FOR` atom.
typedef enum {
  UT_X11_PRIMARY = 1,
  UT_X11_SECONDARY = 2,
  UT_X11_ARC = 3,
  UT_X11_ATOM = 4,
  UT_X11_BITMAP = 5,
  UT_X11_CARDINAL = 6,
  UT_X11_COLORMAP = 7,
  UT_X11_CURSOR = 8,
  UT_X11_CUT_BUFFER0 = 8,
  UT_X11_CUT_BUFFER1 = 9,
  UT_X11_CUT_BUFFER2 = 10,
  UT_X11_CUT_BUFFER3 = 11,
  UT_X11_CUT_BUFFER4 = 12,
  UT_X11_CUT_BUFFER5 = 13,
  UT_X11_CUT_BUFFER6 = 14,
  UT_X11_CUT_BUFFER7 = 16,
  UT_X11_DRAWABLE = 17,
  UT_X11_FONT = 18,
  UT_X11_INTEGER = 19,
  UT_X11_PIXMAP = 20,
  UT_X11_POINT = 21,
  UT_X11_RECTANGLE = 22,
  UT_X11_RESOURCE_MANAGER = 23,
  UT_X11_RGB_COLOR_MAP = 24,
  UT_X11_RGB_BEST_MAP = 25,
  UT_X11_RGB_BLUE_MAP = 26,
  UT_X11_RGB_DEFAULT_MAP = 27,
  UT_X11_RGB_GRAY_MAP = 28,
  UT_X11_RGB_GREEN_MAP = 29,
  UT_X11_RGB_RED_MAP = 30,
  UT_X11_STRING = 31,
  UT_X11_VISUALID = 32,
  UT_X11_WINDOW = 33,
  UT_X11_WM_COMMAND = 34,
  UT_X11_WM_HINTS = 35,
  UT_X11_WM_CLIENT_MACHINE = 36,
  UT_X11_WM_ICON_NAME = 37,
  UT_X11_WM_ICON_SIZE = 38,
  UT_X11_WM_NAME = 39,
  UT_X11_WM_NORMAL_HINTS = 40,
  UT_X11_WM_SIZE_HINTS = 41,
  UT_X11_WM_ZOOM_HINTS = 42,
  UT_X11_MIN_SPACE = 43,
  UT_X11_NORM_SPACE = 44,
  UT_X11_MAX_SPACE = 45,
  UT_X11_END_SPACE = 46,
  UT_X11_SUPERSCRIPT_X = 47,
  UT_X11_SUPERSCRIPT_Y = 48,
  UT_X11_SUBSCRIPT_X = 49,
  UT_X11_SUBSCRIPT_Y = 50,
  UT_X11_UNDERLINE_POSITION = 51,
  UT_X11_UNDERLINE_THICKNESS = 52,
  UT_X11_STRIKEOUT_ASCENT = 53,
  UT_X11_STRIKEOUT_DESCENT = 54,
  UT_X11_ITALIC_ANGLE = 55,
  UT_X11_X_HEIGHT = 56,
  UT_X11_QUAD_WIDTH = 57,
  UT_X11_WEIGHT = 58,
  UT_X11_POINT_SIZE = 59,
  UT_X11_RESOLUTION = 60,
  UT_X11_COPYRIGHT = 61,
  UT_X11_NOTICE = 62,
  UT_X11_FONT_NAME = 63,
  UT_X11_FAMILY_NAME = 64,
  UT_X11_FULL_NAME = 65,
  UT_X11_CAP_HEIGHT = 66,
  UT_X11_WM_CLASS = 67,
  UT_X11_WM_TRANSIENT_FOR = 68
} UtX11BuiltinAtom;

/// State of X11 property notify event.
/// - [UT_X11_PROPERTY_NOTIFY_STATE_NEW_VALUE] - Property set to new value.
/// - [UT_X11_PROPERTY_NOTIFY_STATE_DELETE] - Property deleted.
typedef enum {
  UT_X11_PROPERTY_NOTIFY_STATE_NEW_VALUE = 0,
  UT_X11_PROPERTY_NOTIFY_STATE_DELETE = 1
} UtX11PropertyNotifyState;

/// Reason for X11 device changed event:
/// - [UT_X11_DEVICE_CHANGE_REASON_SLAVE_SWITCH] - Slave switch.
/// - [UT_X11_DEVICE_CHANGE_REASON_DEVICE_CHANGE] - Device change.
typedef enum {
  UT_X11_DEVICE_CHANGE_REASON_SLAVE_SWITCH = 1,
  UT_X11_DEVICE_CHANGE_REASON_DEVICE_CHANGE = 2
} UtX11DeviceChangeReason;

/// X11 input notify event mode:
/// - [UT_X11_INPUT_NOTIFY_MODE_NORMAL] - Normal.
/// - [UT_X11_INPUT_NOTIFY_MODE_GRAB] - Grab.
/// - [UT_X11_INPUT_NOTIFY_MODE_UNGRAB] - Ungrab.
/// - [UT_X11_INPUT_NOTIFY_MODE_WHILE_GRABBED] - While grabbed.
/// - [UT_X11_INPUT_NOTIFY_MODE_PASSIVE_GRAB] - Passive grab.
/// - [UT_X11_INPUT_NOTIFY_MODE_PASSIVE_UNGRAB] - Passive ungrab.
typedef enum {
  UT_X11_INPUT_NOTIFY_MODE_NORMAL = 0,
  UT_X11_INPUT_NOTIFY_MODE_GRAB = 1,
  UT_X11_INPUT_NOTIFY_MODE_UNGRAB = 2,
  UT_X11_INPUT_NOTIFY_MODE_WHILE_GRABBED = 3,
  UT_X11_INPUT_NOTIFY_MODE_PASSIVE_GRAB = 4,
  UT_X11_INPUT_NOTIFY_MODE_PASSIVE_UNGRAB = 5
} UtX11InputNotifyMode;

/// Detail in X11 input notify event:
/// - [UT_X11_INPUT_NOTIFY_DETAIL_ANCESTOR] - Ancestor.
/// - [UT_X11_INPUT_NOTIFY_DETAIL_VIRTUAL] - Virtual.
/// - [UT_X11_INPUT_NOTIFY_DETAIL_INFERIOR] - Inferior.
/// - [UT_X11_INPUT_NOTIFY_DETAIL_NONLINEAR] - Nonlinear.
/// - [UT_X11_INPUT_NOTIFY_DETAIL_NONLINEAR_VIRTUAL] - Nonlinear virtual.
/// - [UT_X11_INPUT_NOTIFY_DETAIL_POINTER] - Pointer.
/// - [UT_X11_INPUT_NOTIFY_DETAIL_POINTER_ROOT] - Pointe root.
/// - [UT_X11_INPUT_NOTIFY_DETAIL_NONE] - None.
typedef enum {
  UT_X11_INPUT_NOTIFY_DETAIL_ANCESTOR = 0,
  UT_X11_INPUT_NOTIFY_DETAIL_VIRTUAL = 1,
  UT_X11_INPUT_NOTIFY_DETAIL_INFERIOR = 2,
  UT_X11_INPUT_NOTIFY_DETAIL_NONLINEAR = 3,
  UT_X11_INPUT_NOTIFY_DETAIL_NONLINEAR_VIRTUAL = 4,
  UT_X11_INPUT_NOTIFY_DETAIL_POINTER = 5,
  UT_X11_INPUT_NOTIFY_DETAIL_POINTER_ROOT = 6,
  UT_X11_INPUT_NOTIFY_DETAIL_NONE = 7
} UtX11InputNotifyDetail;

/// Flags in X11 key events:
/// - [UT_X11_KEY_EVENT_FLAG_REPEAT] - event is a key repeat.
typedef enum { UT_X11_KEY_EVENT_FLAG_REPEAT = 0x00010000 } UtX11KeyEventFlag;

/// Flags on X11 pointer events:
/// [UT_X11_POINTER_EVENT_FLAG_EMULATED] - This event is emulated.
typedef enum {
  UT_X11_POINTER_EVENT_FLAG_EMULATED = 0x00010000
} UtX11PointerEventFlag;

/// Flags on X11 touch events:
/// - [UT_X11_TOUCH_EVENT_FLAG_PENDING_END] - Pending end.
/// - [UT_X11_TOUCH_EVENT_FLAG_EMULATING_POINTER] - Emulating pointer.
typedef enum {
  UT_X11_TOUCH_EVENT_FLAG_PENDING_END = 0x00010000,
  UT_X11_TOUCH_EVENT_FLAG_EMULATING_POINTER = 0x00020000
} UtX11TouchEventFlag;

typedef void (*UtX11ExposeCallback)(UtObject *object, uint32_t window,
                                    uint16_t x, uint16_t y, uint16_t width,
                                    uint16_t height, uint16_t count);
typedef void (*UtX11NoExposeCallback)(UtObject *object, uint32_t drawable);
typedef void (*UtX11MapNotifyCallback)(UtObject *object, uint32_t event,
                                       uint32_t window, bool override_redirect);
typedef void (*UtX11ReparentNotifyCallback)(UtObject *object, int32_t event,
                                            uint32_t window, uint32_t parent,
                                            int16_t x, int16_t y,
                                            bool override_redirect);
typedef void (*UtX11ConfigureNotifyCallback)(UtObject *object, uint32_t window,
                                             int16_t x, int16_t y,
                                             uint16_t width, uint16_t height);
typedef void (*UtX11PropertyNotifyCallback)(UtObject *object, uint32_t window,
                                            uint32_t atom, uint32_t timestamp,
                                            UtX11PropertyNotifyState state);
typedef void (*UtX11ClientMessageCallback)(UtObject *object, uint32_t window,
                                           uint32_t type, UtObject *data);

// From XInputExtension
typedef void (*UtX11DeviceChangedCallback)(UtObject *object, uint16_t device_id,
                                           uint32_t timestamp,
                                           UtX11DeviceChangeReason reason,
                                           UtObject *classes);
typedef void (*UtX11KeyPressCallback)(UtObject *object, uint16_t device_id,
                                      uint32_t timestamp, uint32_t window,
                                      uint8_t keycode, double x, double y,
                                      UtX11KeyEventFlag flags);
typedef void (*UtX11KeyReleaseCallback)(UtObject *object, uint16_t device_id,
                                        uint32_t timestamp, uint32_t window,
                                        uint8_t keycode, double x, double y,
                                        UtX11KeyEventFlag flags);
typedef void (*UtX11ButtonPressCallback)(UtObject *object, uint16_t device_id,
                                         uint32_t timestamp, uint32_t window,
                                         uint8_t button, double x, double y,
                                         UtX11PointerEventFlag flags);
typedef void (*UtX11ButtonReleaseCallback)(UtObject *object, uint16_t device_id,
                                           uint32_t timestamp, uint32_t window,
                                           uint8_t button, double x, double y,
                                           UtX11PointerEventFlag flags);
typedef void (*UtX11MotionCallback)(UtObject *object, uint16_t device_id,
                                    uint32_t timestamp, uint32_t window,
                                    double x, double y,
                                    UtX11PointerEventFlag flags);
typedef void (*UtX11EnterCallback)(UtObject *object, uint16_t device_id,
                                   uint32_t timestamp,
                                   UtX11InputNotifyMode mode,
                                   UtX11InputNotifyDetail detail,
                                   uint32_t window, double x, double y);
typedef void (*UtX11LeaveCallback)(UtObject *object, uint16_t device_id,
                                   uint32_t timestamp,
                                   UtX11InputNotifyMode mode,
                                   UtX11InputNotifyDetail detail,
                                   uint32_t window, double x, double y);
typedef void (*UtX11FocusInCallback)(UtObject *object, uint16_t device_id,
                                     uint32_t timestamp,
                                     UtX11InputNotifyMode mode,
                                     UtX11InputNotifyDetail detail,
                                     uint32_t window);
typedef void (*UtX11FocusOutCallback)(UtObject *object, uint16_t device_id,
                                      uint32_t timestamp,
                                      UtX11InputNotifyMode mode,
                                      UtX11InputNotifyDetail detail,
                                      uint32_t window);
typedef void (*UtX11TouchBeginCallback)(UtObject *object, uint16_t device_id,
                                        uint32_t timestamp, uint32_t window,
                                        uint32_t touch_id, double x, double y);
typedef void (*UtX11TouchUpdateCallback)(UtObject *object, uint16_t device_id,
                                         uint32_t timestamp, uint32_t window,
                                         uint32_t touch_id, double x, double y);
typedef void (*UtX11TouchEndCallback)(UtObject *object, uint16_t device_id,
                                      uint32_t timestamp, uint32_t window,
                                      uint32_t touch_id, double x, double y);

// From XFIXES
typedef void (*UtX11SelectionNotifyCallback)(UtObject *object, uint32_t window,
                                             uint32_t owner, uint32_t selection,
                                             uint32_t timestamp,
                                             uint32_t selection_timestamp);
typedef void (*UtX11CursorNotifyCallback)(UtObject *object, uint32_t window,
                                          uint32_t cursor_serial,
                                          uint32_t timestamp, uint32_t name);

// From Present
typedef void (*UtX11PresentConfigureNotifyCallback)(
    UtObject *object, uint32_t event_id, uint32_t window, int16_t x, int16_t y,
    uint16_t width, uint16_t height, int16_t off_x, int16_t off_y,
    uint16_t pixmap_width, uint16_t pixmap_height, uint32_t pixmap_flags);
typedef void (*UtX11PresentCompleteNotifyCallback)(
    UtObject *object, uint8_t kind, uint8_t mode, uint32_t event_id,
    uint32_t window, uint32_t serial, uint64_t ust, uint64_t msc);
typedef void (*UtX11PresentIdleNotifyCallback)(UtObject *object,
                                               uint32_t event_id,
                                               uint32_t window, uint32_t serial,
                                               uint32_t pixmap,
                                               uint32_t idle_fence);

typedef void (*UtX11UnknownEventCallback)(UtObject *object, uint8_t code,
                                          bool from_send_event,
                                          uint16_t sequence_number,
                                          uint8_t data0, UtObject *data);
typedef void (*UtX11UnknownGenericEventCallback)(UtObject *object,
                                                 uint8_t major_opcode,
                                                 uint16_t code);

typedef struct {
  UtX11ExposeCallback expose;
  UtX11NoExposeCallback no_expose;
  UtX11MapNotifyCallback map_notify;
  UtX11ReparentNotifyCallback reparent_notify;
  UtX11ConfigureNotifyCallback configure_notify;
  UtX11PropertyNotifyCallback property_notify;
  UtX11ClientMessageCallback client_message;

  // From XInputExtension
  UtX11DeviceChangedCallback device_changed;
  UtX11KeyPressCallback key_press;
  UtX11KeyReleaseCallback key_release;
  UtX11ButtonPressCallback button_press;
  UtX11ButtonReleaseCallback button_release;
  UtX11MotionCallback motion;
  UtX11EnterCallback enter;
  UtX11LeaveCallback leave;
  UtX11FocusInCallback focus_in;
  UtX11FocusOutCallback focus_out;
  UtX11TouchBeginCallback touch_begin;
  UtX11TouchUpdateCallback touch_update;
  UtX11TouchEndCallback touch_end;

  // From XFIXES
  UtX11SelectionNotifyCallback selection_notify;
  UtX11CursorNotifyCallback cursor_notify;

  // From Present
  UtX11PresentConfigureNotifyCallback present_configure_notify;
  UtX11PresentCompleteNotifyCallback present_complete_notify;
  UtX11PresentIdleNotifyCallback present_idle_notify;

  UtX11UnknownEventCallback unknown_event;
  UtX11UnknownGenericEventCallback unknown_generic_event;
} UtX11EventCallbacks;

typedef void (*UtX11ClientErrorCallback)(UtObject *object, UtObject *error);
typedef void (*UtX11ClientConnectCallback)(UtObject *object, UtObject *error);
typedef void (*UtX11GetWindowAttributesCallback)(
    UtObject *object, uint32_t visual, uint16_t class, uint8_t bit_gravity,
    uint8_t win_gravity, uint8_t backing_store, uint32_t backing_planes,
    uint32_t backing_pixel, bool save_under, bool map_is_installed,
    uint8_t map_state, bool override_redirect, uint32_t colormap,
    uint32_t all_event_masks, uint32_t your_event_mask,
    uint16_t do_not_propagate_mask, UtObject *error);
typedef void (*UtX11InternAtomCallback)(UtObject *object, uint32_t atom,
                                        UtObject *error);
typedef void (*UtX11GetAtomNameCallback)(UtObject *object, const char *name,
                                         UtObject *error);
typedef void (*UtX11GetPropertyCallback)(UtObject *object, uint32_t type,
                                         UtObject *value, uint32_t bytes_after,
                                         UtObject *error);
typedef void (*UtX11ListPropertiesCallback)(UtObject *object, UtObject *atoms,
                                            UtObject *error);
typedef void (*UtX11GetImageCallback)(UtObject *object, uint32_t visual,
                                      uint8_t depth, UtObject *data,
                                      UtObject *error);
typedef void (*UtX11QueryExtensionCallback)(UtObject *object, bool present,
                                            uint8_t major_opcode,
                                            uint8_t first_event,
                                            uint8_t first_error,
                                            UtObject *error);
typedef void (*UtX11ListExtensionsCallback)(UtObject *object, UtObject *names,
                                            UtObject *error);
typedef void (*UtX11ShmCreateSegmentCallback)(UtObject *object, UtObject *fd,
                                              UtObject *error);
typedef void (*UtX11GrabDeviceCallback)(UtObject *object, uint8_t status,
                                        UtObject *error);
typedef void (*UtX11QueryDeviceCallback)(UtObject *object, UtObject *infos,
                                         UtObject *error);
typedef void (*UtX11GetFocusCallback)(UtObject *object, uint32_t window,
                                      UtObject *error);
typedef void (*UtX11ListSystemCountersCallback)(UtObject *object,
                                                UtObject *counters,
                                                UtObject *error);
typedef void (*UtX11QueryCounterCallback)(UtObject *object,
                                          int64_t counter_value,
                                          UtObject *error);

/// X11 events:
/// - [UT_X11_EVENT_EXPOSURE] - Exposure event.
/// - [UT_X11_EVENT_VISIBILITY_CHANGE] - Visibility change event.
/// - [UT_X11_EVENT_STRUCTURE_NOTIFY] - Structure notify event.
/// - [UT_X11_EVENT_RESIZE_REDIRECT] - Resize redirect event.
/// - [UT_X11_EVENT_SUBSTRUCTURE_NOTIFY] - Substructure notify event.
/// - [UT_X11_EVENT_SUBSTRUCTURE_REDIRECT] - Substructure redirect event.
/// - [UT_X11_EVENT_FOCUS_CHANGE] - Focus change event.
/// - [UT_X11_EVENT_PROPERTY_CHANGE] - Propert change event.
/// - [UT_X11_EVENT_COLORMAP_CHANGE] - Colormap cange event.
/// - [UT_X11_EVENT_OWNER_GRAB_BUTTON] - Owner grab button event.
typedef enum {
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

#define UT_X11_DEVICE_ALL 0
#define UT_X11_DEVICE_ALL_MASTER 1

/// X11 input events:
/// - [UT_X11_INPUT_EVENT_DEVICE_CHANGED] - Device changed event.
/// - [UT_X11_INPUT_EVENT_KEY_PRESS] - Key press event.
/// - [UT_X11_INPUT_EVENT_KEY_RELEASE] - Key release event.
/// - [UT_X11_INPUT_EVENT_BUTTON_PRESS] - Button press event.
/// - [UT_X11_INPUT_EVENT_BUTTON_RELEASE] - Button release event.
/// - [UT_X11_INPUT_EVENT_MOTION] - Motion event.
/// - [UT_X11_INPUT_EVENT_ENTER] - Enter event.
/// - [UT_X11_INPUT_EVENT_LEAVE] - Leave event.
/// - [UT_X11_INPUT_EVENT_FOCUS_IN] - Focus in event.
/// - [UT_X11_INPUT_EVENT_FOCUS_OUT] - Focus out event.
/// - [UT_X11_INPUT_EVENT_HIERARCHY] - Hierachy event.
/// - [UT_X11_INPUT_EVENT_PROPERTY] - Property event.
/// - [UT_X11_INPUT_EVENT_RAW_KEY_PRESS] - Raw key press event.
/// - [UT_X11_INPUT_EVENT_RAW_KEY_RELEASE] - Raw key release event.
/// - [UT_X11_INPUT_EVENT_RAW_BUTTON_PRESS] - Raw button press event.
/// - [UT_X11_INPUT_EVENT_RAW_BUTTON_RELEASE] - Raw button release event.
/// - [UT_X11_INPUT_EVENT_RAW_MOTION] - Raw motion event.
/// - [UT_X11_INPUT_EVENT_TOUCH_BEGIN] - Touch begin event.
/// - [UT_X11_INPUT_EVENT_TOUCH_UPDATE] - Touch update event.
/// - [UT_X11_INPUT_EVENT_TOUCH_END] - Touch end event.
/// - [UT_X11_INPUT_EVENT_TOUCH_OWNERSHIP] - Touch ownership event.
/// - [UT_X11_INPUT_EVENT_RAW_TOUCH_BEGIN] - Raw touch being event.
/// - [UT_X11_INPUT_EVENT_RAW_TOUCH_UPDATE] - Raw touch update event.
/// - [UT_X11_INPUT_EVENT_RAW_TOUCH_END] - Raw touch end event.
/// - [UT_X11_INPUT_EVENT_BARRIER_HIT] - Barrier hit event.
/// - [UT_X11_INPUT_EVENT_BARRIER_LEAVE] - Barrier leave event.
/// - [UT_X11_INPUT_EVENT_GESTURE_PINCH_BEGIN] - Gesture pinch begin event.
/// - [UT_X11_INPUT_EVENT_GESTURE_PINCH_UPDATE] - Gesture pinch update event.
/// - [UT_X11_INPUT_EVENT_GESTURE_PINCH_END] - Gesture pinch end event.
/// - [UT_X11_INPUT_EVENT_SWIPE_PINCH_BEGIN] - Switch pinch begin event.
/// - [UT_X11_INPUT_EVENT_SWIPE_PINCH_UPDATE] - Switch pinch update event.
/// - [UT_X11_INPUT_EVENT_SWIPE_PINCH_END] - Switch pinch end event.
typedef enum {
  UT_X11_INPUT_EVENT_DEVICE_CHANGED = 0x00000002,
  UT_X11_INPUT_EVENT_KEY_PRESS = 0x00000004,
  UT_X11_INPUT_EVENT_KEY_RELEASE = 0x00000008,
  UT_X11_INPUT_EVENT_BUTTON_PRESS = 0x00000010,
  UT_X11_INPUT_EVENT_BUTTON_RELEASE = 0x00000020,
  UT_X11_INPUT_EVENT_MOTION = 0x00000040,
  UT_X11_INPUT_EVENT_ENTER = 0x00000080,
  UT_X11_INPUT_EVENT_LEAVE = 0x00000100,
  UT_X11_INPUT_EVENT_FOCUS_IN = 0x00000200,
  UT_X11_INPUT_EVENT_FOCUS_OUT = 0x00000400,
  UT_X11_INPUT_EVENT_HIERARCHY = 0x00000800,
  UT_X11_INPUT_EVENT_PROPERTY = 0x00001000,
  UT_X11_INPUT_EVENT_RAW_KEY_PRESS = 0x00002000,
  UT_X11_INPUT_EVENT_RAW_KEY_RELEASE = 0x00004000,
  UT_X11_INPUT_EVENT_RAW_BUTTON_PRESS = 0x00008000,
  UT_X11_INPUT_EVENT_RAW_BUTTON_RELEASE = 0x00010000,
  UT_X11_INPUT_EVENT_RAW_MOTION = 0x00020000,
  UT_X11_INPUT_EVENT_TOUCH_BEGIN = 0x00040000,
  UT_X11_INPUT_EVENT_TOUCH_UPDATE = 0x00080000,
  UT_X11_INPUT_EVENT_TOUCH_END = 0x00100000,
  UT_X11_INPUT_EVENT_TOUCH_OWNERSHIP = 0x00200000,
  UT_X11_INPUT_EVENT_RAW_TOUCH_BEGIN = 0x00400000,
  UT_X11_INPUT_EVENT_RAW_TOUCH_UPDATE = 0x00800000,
  UT_X11_INPUT_EVENT_RAW_TOUCH_END = 0x01000000,
  UT_X11_INPUT_EVENT_BARRIER_HIT = 0x02000000,
  UT_X11_INPUT_EVENT_BARRIER_LEAVE = 0x04000000,
  UT_X11_INPUT_EVENT_GESTURE_PINCH_BEGIN = 0x08000000,
  UT_X11_INPUT_EVENT_GESTURE_PINCH_UPDATE = 0x10000000,
  UT_X11_INPUT_EVENT_GESTURE_PINCH_END = 0x20000000,
  UT_X11_INPUT_EVENT_SWIPE_PINCH_BEGIN = 0x40000000,
  UT_X11_INPUT_EVENT_SWIPE_PINCH_UPDATE = 0x80000000,
  UT_X11_INPUT_EVENT_SWIPE_PINCH_END =
      0x100000000 // FIXME: Greater than 64 bits
} UtX11InputEvent;

/// X11 image formast:
/// - [UT_X11_IMAGE_FORMAT_BITMAP] - Bitmap.
/// - [UT_X11_IMAGE_FORMAT_XY_PIXMAP] - XY pixmap.
/// - [UT_X11_IMAGE_FORMAT_Z_PIXMAP] - Z pixmap.
typedef enum {
  UT_X11_IMAGE_FORMAT_BITMAP,
  UT_X11_IMAGE_FORMAT_XY_PIXMAP,
  UT_X11_IMAGE_FORMAT_Z_PIXMAP
} UtX11ImageFormat;

/// Mode when changing X11 properites.
/// - [UT_X11_PROPERTY_MODE_REPLACE] - Replace this property.
/// - [UT_X11_PROPERTY_MODE_PREPEND] - Prepend this property.
/// - [UT_X11_PROPERTY_MODE_APPEND] - Append this property.
typedef enum {
  UT_X11_PROPERTY_MODE_REPLACE = 0,
  UT_X11_PROPERTY_MODE_PREPEND = 1,
  UT_X11_PROPERTY_MODE_APPEND = 2
} UtX11PropertyMode;

/// Creates a new X11 client.
/// [event_callbacks] should be set with functions for the events you wish to
/// consume. [error_callback] is used to detect errors returns from the X
/// server.
///
/// !return-ref
/// !return-type UtX11Client
UtObject *ut_x11_client_new(UtObject *callback_object,
                            const UtX11EventCallbacks *event_callbacks,
                            UtX11ClientErrorCallback error_callback);

/// Connect to the X server.
/// [callback] is called when the connection is completed.
void ut_x11_client_connect(UtObject *object, UtObject *callback_object,
                           UtX11ClientConnectCallback callback);

/// Returns the X11 server vendor.
/// This value is set once [ut_x11_client_connect] has completed.
const char *ut_x11_client_get_vendor(UtObject *object);

/// Returns the pixmap formats supported by the X server.
/// This value is set once [ut_x11_client_connect] has completed.
///
/// !return-type UtList
/// !return-element-type UtX11PixmapFormat
UtObject *ut_x11_client_get_pixmap_formats(UtObject *object);

/// Returns the screens provided by the X server.
/// This value is set once [ut_x11_client_connect] has completed.
///
/// !return-type UtList
/// !return-element-type UtX11Screen
UtObject *ut_x11_client_get_screens(UtObject *object);

/// Returns a new ID for a window of dimensions [width]x[height] pixels and
/// offset [x],[y] pixels from the root window.
uint32_t ut_x11_client_create_window(UtObject *object, int16_t x, int16_t y,
                                     uint16_t width, uint16_t height,
                                     uint32_t event_mask);

/// Change attributes on [window].
void ut_x11_client_change_window_attributes(UtObject *object, uint32_t window,
                                            uint32_t event_mask);

/// Get attributes on [window].
void ut_x11_client_get_window_attributes(
    UtObject *object, uint32_t window, UtObject *callback_object,
    UtX11GetWindowAttributesCallback callback);

/// Destroy [window].
void ut_x11_client_destroy_window(UtObject *object, uint32_t window);

/// Destroy subwindows of [window].
void ut_x11_client_destroy_subwindows(UtObject *object, uint32_t window);

/// Move [window] to be a child of [parent] at position [x],[y] pixels.
void ut_x11_client_reparent_window(UtObject *object, uint32_t window,
                                   uint32_t parent, int16_t x, int16_t y);

/// Map [window].
void ut_x11_client_map_window(UtObject *object, uint32_t window);

/// Map subwindows of [window].
void ut_x11_client_map_subwindows(UtObject *object, uint32_t window);

/// Unmap [window].
void ut_x11_client_unmap_window(UtObject *object, uint32_t window);

/// Unmap subwindows of [window].
void ut_x11_client_unmap_subwindows(UtObject *object, uint32_t window);

/// Configure [window] to have dimensions [width]x[height] pixels and offset
/// [x],[y] pixels from its parent.
void ut_x11_client_configure_window(UtObject *object, uint32_t window,
                                    int16_t x, int16_t y, uint16_t width,
                                    uint16_t height);

/// Intern an atom with [name].
void ut_x11_client_intern_atom(UtObject *object, const char *name,
                               bool only_if_exists, UtObject *callback_object,
                               UtX11InternAtomCallback callback);

/// Gets the name of [atom].
void ut_x11_client_get_atom_name(UtObject *object, uint32_t atom,
                                 UtObject *callback_object,
                                 UtX11GetAtomNameCallback callback);

/// Change [property] of [window].
void ut_x11_client_change_property_uint8(UtObject *object, uint32_t window,
                                         uint32_t property,
                                         UtX11PropertyMode mode, uint32_t type,
                                         UtObject *value);

/// Change [property] of [window].
void ut_x11_client_change_property_uint16(UtObject *object, uint32_t window,
                                          uint32_t property,
                                          UtX11PropertyMode mode, uint32_t type,
                                          UtObject *value);

/// Change [property] of [window].
void ut_x11_client_change_property_uint32(UtObject *object, uint32_t window,
                                          uint32_t property,
                                          UtX11PropertyMode mode, uint32_t type,
                                          UtObject *value);

/// Change [property] of [window].
void ut_x11_client_change_property_string(UtObject *object, uint32_t window,
                                          uint32_t property,
                                          UtX11PropertyMode mode,
                                          const char *value);

/// Delete [property] of [window].
void ut_x11_client_delete_property(UtObject *object, uint32_t window,
                                   uint32_t property);

/// Get [property] of [window].
void ut_x11_client_get_property(UtObject *object, uint32_t window,
                                uint32_t property, uint32_t type,
                                UtObject *callback_object,
                                UtX11GetPropertyCallback callback);

/// Get [property] of [window].
void ut_x11_client_get_property_full(UtObject *object, uint32_t window,
                                     uint32_t property, uint32_t type,
                                     uint32_t long_offset, uint32_t long_length,
                                     bool delete, UtObject *callback_object,
                                     UtX11GetPropertyCallback callback);

/// Get properties of [window].
void ut_x11_client_list_properties(UtObject *object, uint32_t window,
                                   UtObject *callback_object,
                                   UtX11ListPropertiesCallback callback);

/// Returns a new ID for a pixmap of dimentions [witdth]x[height] pixels.
uint32_t ut_x11_client_create_pixmap(UtObject *object, uint32_t drawable,
                                     uint16_t width, uint16_t height,
                                     uint8_t depth);

/// Frees [pixmap].
void ut_x11_client_free_pixmap(UtObject *object, uint32_t pixmap);

/// Returns a new ID for a graphics context used to access [drawable].
uint32_t ut_x11_client_create_gc(UtObject *object, uint32_t drawable);

/// Frees [gc].
void ut_x11_client_free_gc(UtObject *object, uint32_t gc);

/// Clears rectangle of size [width]x[height] at position [x],[y] on [window].
/// If [exposures] is true one or more exposure events are generated for this
/// area.
void ut_x11_client_clear_area(UtObject *object, uint32_t window, int16_t x,
                              int16_t y, uint16_t width, uint16_t height,
                              bool exposures);

/// Copies a rectangle of size [width]x[height] from [src_drawable] to
/// [dst_drawable].
void ut_x11_client_copy_area(UtObject *object, uint32_t src_drawable,
                             uint32_t dst_drawable, uint32_t gc, int16_t src_x,
                             int16_t src_y, int16_t dst_x, int16_t dst_y,
                             uint16_t width, uint16_t height);

/// Gets an image from [drawable].
/// Calls [callback] with the returned image.
///
/// !arg-type data UtUint8List
void ut_x11_client_get_image(UtObject *object, uint32_t drawable,
                             UtX11ImageFormat format, int16_t x, int16_t y,
                             uint16_t width, uint16_t height,
                             uint32_t plane_mask, UtObject *callback_object,
                             UtX11GetImageCallback callback);

/// Puts an image contained in [data] into [drawable].
///
/// !arg-type data UtUint8List
void ut_x11_client_put_image(UtObject *object, uint32_t drawable, uint32_t gc,
                             UtX11ImageFormat format, uint16_t width,
                             uint16_t height, uint8_t depth, int16_t dst_x,
                             int16_t dst_y, UtObject *data);

/// Query if the extension with [name] is available.
void ut_x11_client_query_extension(UtObject *object, const char *name,
                                   UtObject *callback_object,
                                   UtX11QueryExtensionCallback callback);

/// Get the extensions available on the X server.
void ut_x11_client_list_extensions(UtObject *object, UtObject *callback_object,
                                   UtX11ListExtensionsCallback callback);

/// Trigger a bell event.
void ut_x11_client_bell(UtObject *object);

/// Kill the client that owns [resource].
void ut_x11_client_kill_client(UtObject *object, uint32_t resource);

/// Returns a new shared memory segment ID for shared memory in [shmid].
uint32_t ut_x11_client_shm_attach(UtObject *object, uint32_t shmid,
                                  bool read_only);

/// Detached a shared memory [segment].
void ut_x11_client_shm_detach(UtObject *object, uint32_t segment);

/// Returns a new pixmap of size [width]x[height] and [depth] created from a
/// shared memory [segment]. The pixmap data starts at [offset] bytes in the
/// segment.
uint32_t ut_x11_client_shm_create_pixmap(UtObject *object, uint32_t drawable,
                                         uint16_t width, uint16_t height,
                                         uint8_t depth, uint32_t segment,
                                         uint32_t offset);

/// Attaches a shared memory [fd].
///
/// !arg-type fd UtFileDescriptor
uint32_t ut_x11_client_shm_attach_fd(UtObject *object, UtObject *fd,
                                     bool read_only);

/// Returns a new shared memory segment of [size] bytes from the memory applied
/// in [ut_x11_client_shm_attach_fd].
uint32_t
ut_x11_client_shm_create_segment(UtObject *object, uint32_t size,
                                 bool read_only, UtObject *callback_object,
                                 UtX11ShmCreateSegmentCallback callback);

/// Select the input event [masks] to be applied to window.
///
/// !arg-type masks UtObjectList
void ut_x11_client_select_input_events(UtObject *object, uint32_t window,
                                       UtObject *masks);

/// Queries input device with [device_id].
void ut_x11_client_query_device(UtObject *object, uint16_t device_id,
                                UtObject *callback_object,
                                UtX11QueryDeviceCallback callback);

/// Sets the [device_id] that is in focus for [window].
void ut_x11_client_set_focus(UtObject *object, uint32_t window,
                             uint32_t timestamp, uint16_t device_id);

/// Gets the device that is in focus for [window].
/// Calls [callback] when the device is returned.
void ut_x11_client_get_focus(UtObject *object, uint16_t device_id,
                             UtObject *callback_object,
                             UtX11GetFocusCallback callback);

/// Get the system counters on the X server.
void ut_x11_client_list_system_counters(
    UtObject *object, UtObject *callback_object,
    UtX11ListSystemCountersCallback callback);

/// Returns the ID for a new counter with [initial_value].
uint32_t ut_x11_client_create_counter(UtObject *object, int64_t initial_value);

/// Set the [amount] of [counter].
void ut_x11_client_set_counter(UtObject *object, uint32_t counter,
                               int64_t amount);

/// Change the [amount] of [counter].
void ut_x11_client_change_counter(UtObject *object, uint32_t counter,
                                  int64_t amount);

/// Query information about [counter].
void ut_x11_client_query_counter(UtObject *object, uint32_t counter,
                                 UtObject *callback_object,
                                 UtX11QueryCounterCallback callback);

/// Destroy [counter].
void ut_x11_client_destroy_counter(UtObject *object, uint32_t counter);

/// Returns [true] if [object] is a [UtX11Client].
bool ut_object_is_x11_client(UtObject *object);
