#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef void (*UtX11ClientXfixesQueryVersionCallback)(void *user_data,
                                                      uint32_t major_version,
                                                      uint32_t minor_version,
                                                      UtObject *error);
typedef void (*UtX11ClientXfixesGetCursorImageCallback)(
    void *user_data, int16_t x, int16_t y, uint16_t width, uint16_t height,
    uint16_t xhot, uint16_t yhot, uint32_t cursor_serial,
    UtObject *cursor_image, UtObject *error);
typedef void (*UtX11ClientXfixesGetCursorNameCallback)(void *user_data,
                                                       uint32_t atom,
                                                       const char *name,
                                                       UtObject *error);
typedef void (*UtX11ClientXfixesGetCursorImageAndNameCallback)(
    void *user_data, int16_t x, int16_t y, uint16_t width, uint16_t height,
    uint16_t xhot, uint16_t yhot, uint32_t cursor_serial,
    UtObject *cursor_image, uint32_t cursor_atom, const char *name,
    UtObject *error);

UtObject *
ut_x11_xfixes_extension_new(UtObject *client, uint8_t major_opcode,
                            uint8_t first_event, uint8_t first_error,
                            const UtX11EventCallbacks *event_callbacks,
                            void *user_data, UtObject *cancel);

void ut_x11_xfixes_extension_query_version(
    UtObject *object, UtX11ClientXfixesQueryVersionCallback callback,
    void *user_data, UtObject *cancel);

// ChangeSaveSet

void ut_x11_xfixes_select_selection_input(UtObject *object, uint32_t window,
                                          uint32_t selection,
                                          uint32_t event_mask);

void ut_x11_xfixes_select_cursor_input(UtObject *object, uint32_t window,
                                       uint32_t event_mask);

void ut_x11_xfixes_extension_get_cursor_image(
    UtObject *object, UtX11ClientXfixesGetCursorImageCallback callback,
    void *user_data, UtObject *cancel);

uint32_t ut_x11_xfixes_create_region(UtObject *object, UtObject *rectangles);

uint32_t ut_x11_xfixes_create_region_from_bitmap(UtObject *object,
                                                 uint32_t bitmap);

uint32_t ut_x11_xfixes_create_region_from_window(UtObject *object,
                                                 uint32_t window, uint8_t kind);

uint32_t ut_x11_xfixes_create_region_from_gc(UtObject *object, uint32_t gc);

uint32_t ut_x11_xfixes_create_region_from_picture(UtObject *object,
                                                  uint32_t picture);

void ut_x11_xfixes_destroy_region(UtObject *object, uint32_t region);

void ut_x11_xfixes_set_region(UtObject *object, uint32_t region,
                              UtObject *rectangles);

void ut_x11_xfixes_copy_region(UtObject *object, uint32_t source,
                               uint32_t destination);

void ut_x11_xfixes_union_region(UtObject *object, uint32_t source1,
                                uint32_t source2, uint32_t destination);

void ut_x11_xfixes_intersect_region(UtObject *object, uint32_t source1,
                                    uint32_t source2, uint32_t destination);

void ut_x11_xfixes_subtract_region(UtObject *object, uint32_t source1,
                                   uint32_t source2, uint32_t destination);

void ut_x11_xfixes_invert_region(UtObject *object, uint32_t source,
                                 uint32_t bounds, uint32_t destination);

void ut_x11_xfixes_translate_region(UtObject *object, uint32_t region,
                                    int16_t dx, int16_t dy);

void ut_x11_xfixes_region_extents(UtObject *object, uint32_t source,
                                  uint32_t destination);

// FetchRegion

void ut_x11_xfixes_set_gc_clip_region(UtObject *object, uint32_t gc,
                                      uint32_t region, int16_t x_origin,
                                      int16_t y_origin);

void ut_x11_xfixes_set_window_shape_region(UtObject *object, uint32_t dest,
                                           uint8_t kind, int16_t x_offset,
                                           int16_t y_offset, uint32_t region);

void ut_x11_xfixes_set_picture_clip_region(UtObject *object, uint32_t picture,
                                           uint32_t region, int16_t x_origin,
                                           int16_t y_origin);

void ut_x11_xfixes_set_cursor_name(UtObject *object, uint32_t cursor,
                                   const char *name);

void ut_x11_xfixes_extension_get_cursor_name(
    UtObject *object, uint32_t cursor,
    UtX11ClientXfixesGetCursorNameCallback callback, void *user_data,
    UtObject *cancel);

void ut_x11_xfixes_extension_get_cursor_image_and_name(
    UtObject *object, UtX11ClientXfixesGetCursorImageAndNameCallback callback,
    void *user_data, UtObject *cancel);

void ut_x11_xfixes_change_cursor(UtObject *object, uint32_t source,
                                 uint32_t destination);

void ut_x11_xfixes_change_cursor_by_name(UtObject *object, uint32_t cursor,
                                         const char *name);

void ut_x11_xfixes_expand_region(UtObject *object, uint32_t source,
                                 uint32_t destination, uint16_t left,
                                 uint16_t right, uint16_t top, uint16_t bottom);

void ut_x11_xfixes_hide_cursor(UtObject *object, uint32_t window);

void ut_x11_xfixes_show_cursor(UtObject *object, uint32_t window);

bool ut_object_is_x11_xfixes_extension(UtObject *object);
